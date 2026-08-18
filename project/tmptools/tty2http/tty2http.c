/*
 *    说明:  示例 exe：uart@tty 串口 <-> HTTP POST
 *    作者:  tmptools, zxx@ashyelf.com
 *    公司:  ashyelf
 *
 *  做什么
 *    1. 打开 uart@tty 对应的串口
 *    2. 把串口收到的数据按“空闲间隔 / 最大长度”收成一帧
 *    3. 用 libcurl 把这一帧 POST 到示例 HTTP 服务器
 *    4. 把服务器返回的 HTTP body 写回串口
 *
 *  串口参数和 HTTP 地址都是下面的宏，改完后重新 make obj=tmptools。
 *
 *  电脑上可先起一个回显服务器（HTTP_HOST:HTTP_PORT），用来验证来回：
 *    python3 -c "
 * from http.server import BaseHTTPRequestHandler, HTTPServer
 * class E(BaseHTTPRequestHandler):
 *     def do_POST(self):
 *         n = int(self.headers.get('Content-Length', '0'))
 *         b = self.rfile.read(n)
 *         self.send_response(200)
 *         self.send_header('Content-Type', 'application/octet-stream')
 *         self.send_header('Content-Length', str(len(b)))
 *         self.end_headers()
 *         self.wfile.write(b)
 * HTTPServer(('0.0.0.0', 8080), E).serve_forever()
 * "
 */

#include "skin/skin.h"
#include "skinuart/skinuart.h"
#include <curl/curl.h>
#include <poll.h>


/* 要读的串口对象；优先用它的 ttydev，没有则用 TTY_DEV */
#define TTY_OBJECT          "uart@tty"
#define TTY_DEV             "/dev/ttyS1"

/* 传给 serial_open 的线路参数（键名与 uart@tty 配置相同） */
#define TTY_SPEED           "115200"
#define TTY_DATABIT         "8"
#define TTY_STOPBIT         "1"
#define TTY_PARITY          "disable"
#define TTY_FLOW            "disable"
#define TTY_OPEN_MS         100

/* 组帧：字节间空闲超过此时长，或累计达到此长度，就发一次 HTTP */
#define TTY_FRAME_MS        50
#define TTY_FRAME_MAX       2048

/* 示例服务器：POST http://HTTP_HOST:HTTP_PORT HTTP_PATH */
#define HTTP_HOST           "192.168.8.250"
#define HTTP_PORT           8080
#define HTTP_PATH           "/uart"
#define HTTP_TIMEOUT_S      5
#define HTTP_REPLY_MAX      8192



/* 启动：拉起长期运行的 service 进程 */
boole_t _setup( obj_t this, param_t param )
{
    boole ok;
    const char *object;

    (void)param;
    /* 本 exe 的对象名，例如 tmptools@tty2http */
    object = obj_name( this );
    app_info( "%s: setup, start service", object );
    /* 若尚未运行，则启动 _service；已运行则不会再起一份 */
    ok = cstart( this, "service", NULL, object );
    if ( ok == false )
    {
        app_warn( "%s: cstart service failed", object );
        return tfalse;
    }
    return ttrue;
}

/* 停止：按对象名删掉 service，内核会结束对应进程 */
boole_t _shut( obj_t this, param_t param )
{
    const char *object;

    (void)param;
    object = obj_name( this );
    app_info( "%s: shut", object );
    sdelete( object );
    return ttrue;
}

/* 服务进程本体：开串口 → 收帧 → curl POST → 把 HTTP body 写回串口 */
boole_t _service( obj_t this, param_t param )
{
    int tty_fd;
    int frame_len;
    int wait_ms;
    int pret;
    int nread;
    int need_post;
    int url_len;
    talk_t cfg;
    const char *object;
    const char *ttydev;
    const char *cerr;
    CURL *curl;
    struct curl_slist *hdrs;
    CURLcode cres;
    char path[PATH_MAX];
    char frame[TTY_FRAME_MAX];
    char url[256];
    struct pollfd pfd;

    (void)param;
    object = obj_name( this );
    /* 写 HTTP 时对端若已断开，避免 SIGPIPE 把进程打死 */
    signal( SIGPIPE, SIG_IGN );

    /* 先问 uart@tty 的 ttydev；没有配置就用宏里的设备节点 */
    ttydev = config_sgets_string( path, sizeof(path), TTY_OBJECT, "ttydev" );
    if ( ttydev == NULL || *ttydev == '\0' )
    {
        ttydev = TTY_DEV;
    }

    /* 用宏拼一份 serial_open 需要的线路配置 */
    cfg = json_create( NULL );
    if ( cfg == NULL )
    {
        app_fault( "%s: json_create failed", object );
        return tfalse;
    }
    json_set_string( cfg, "speed", TTY_SPEED );
    json_set_string( cfg, "databit", TTY_DATABIT );
    json_set_string( cfg, "stopbit", TTY_STOPBIT );
    json_set_string( cfg, "parity", TTY_PARITY );
    json_set_string( cfg, "flow", TTY_FLOW );

    /* 打开串口；cfg 只给 open 用，随后即可释放 */
    tty_fd = serial_open( ttydev, cfg, TTY_OPEN_MS );
    talk_free( cfg );
    if ( tty_fd < 0 )
    {
        app_faulting( "%s: serial_open %s failed", object, ttydev );
        return tfalse;
    }
    /* 后面用 poll 等数据，描述符改成非阻塞，避免 read 卡住 */
    fd_nonblock( tty_fd );

    /* 初始化 libcurl 全局环境（整个进程只需一次） */
    cres = curl_global_init( CURL_GLOBAL_DEFAULT );
    if ( cres != CURLE_OK )
    {
        cerr = curl_easy_strerror( cres );
        app_fault( "%s: curl_global_init %s", object, cerr );
        close( tty_fd );
        return tfalse;
    }
    /* 创建一个可反复 perform 的 easy 句柄 */
    curl = curl_easy_init();
    if ( curl == NULL )
    {
        app_fault( "%s: curl_easy_init failed", object );
        curl_global_cleanup();
        close( tty_fd );
        return tfalse;
    }
    /* 把宏拼成完整 URL，后面 CURLOPT_URL 一直用这块缓冲 */
    url_len = snprintf( url, sizeof(url), "http://%s:%d%s", HTTP_HOST, HTTP_PORT, HTTP_PATH );
    if ( url_len <= 0 || url_len >= (int)sizeof(url) )
    {
        app_fault( "%s: HTTP URL too long", object );
        curl_easy_cleanup( curl );
        curl_global_cleanup();
        close( tty_fd );
        return tfalse;
    }
    /* 二进制载荷，显式声明 Content-Type，避免 curl 默认成表单编码 */
    hdrs = curl_slist_append( NULL, "Content-Type: application/octet-stream" );
    curl_easy_setopt( curl, CURLOPT_URL, url );
    curl_easy_setopt( curl, CURLOPT_POST, 1L );
    curl_easy_setopt( curl, CURLOPT_HTTPHEADER, hdrs );
    curl_easy_setopt( curl, CURLOPT_TIMEOUT, (long)HTTP_TIMEOUT_S );
    /* 超时不用信号，避免和本进程其它逻辑打架 */
    curl_easy_setopt( curl, CURLOPT_NOSIGNAL, 1L );

    app_info( "%s: service on %s speed=%s databit=%s stopbit=%s parity=%s flow=%s",
            object, ttydev, TTY_SPEED, TTY_DATABIT, TTY_STOPBIT, TTY_PARITY, TTY_FLOW );
    app_info( "%s: HTTP POST %s", object, url );

    /* 当前帧还是空的；poll 只盯串口是否可读 */
    frame_len = 0;
    pfd.fd = tty_fd;
    pfd.events = POLLIN;

    for ( ;; )
    {
        /* 已有半帧：最多再等 TTY_FRAME_MS，当作帧间隔
         * 还是空的：一直等到串口来数据 */
        if ( frame_len > 0 )
        {
            wait_ms = TTY_FRAME_MS;
        }
        else
        {
            wait_ms = -1;
        }

        pret = poll( &pfd, 1, wait_ms );
        if ( pret < 0 )
        {
            /* 被信号打断，再 poll 一次即可 */
            if ( errno == EINTR )
            {
                continue;
            }
            app_warning( "%s: poll", object );
            break;
        }

        need_post = 0;
        if ( pret == 0 )
        {
            /* 超时且帧里已有数据：空闲间隔到了，这一帧可以发出去 */
            if ( frame_len > 0 )
            {
                need_post = 1;
            }
        }
        else
        {
            /* 串口出错 / 挂断，结束服务（进程退出后由框架决定是否拉起） */
            if ( pfd.revents & ( POLLERR | POLLHUP | POLLNVAL ) )
            {
                app_warn( "%s: serial hangup or error", object );
                break;
            }
            if ( pfd.revents & POLLIN )
            {
                /* 接到当前帧后面，不要覆盖已经攒下的字节 */
                nread = read( tty_fd, frame + frame_len, TTY_FRAME_MAX - frame_len );
                if ( nread < 0 )
                {
                    if ( errno == EINTR || errno == EAGAIN )
                    {
                        continue;
                    }
                    app_warning( "%s: serial read", object );
                    /* EIO 一类：串口设备本身坏了，退出进程 */
                    if ( serial_error( errno ) )
                    {
                        break;
                    }
                    continue;
                }
                if ( nread == 0 )
                {
                    app_warn( "%s: serial EOF", object );
                    break;
                }
                frame_len += nread;
                /* 缓冲区满了，不再等空闲间隔，立刻 POST */
                if ( frame_len >= TTY_FRAME_MAX )
                {
                    need_post = 1;
                }
            }
        }

        if ( need_post )
        {
            int off;
            int nwrite;
            int have;
            int post_len;
            int seek_ret;
            FILE *fp;
            char httpbuf[HTTP_REPLY_MAX];

            /* 先记下本帧长度，立刻把组帧长度清零，下一轮可以边收下一帧 */
            post_len = frame_len;
            frame_len = 0;
            app_info( "%s: POST %d bytes", object, post_len );

            /* curl 默认 WRITEFUNCTION 是 fwrite，把 HTTP body 写到这个临时文件 */
            fp = tmpfile();
            if ( fp == NULL )
            {
                app_warning( "%s: tmpfile", object );
                continue;
            }

            /* 指定 POST 正文（可能含 0x00，所以必须带长度，不能靠 strlen） */
            curl_easy_setopt( curl, CURLOPT_POSTFIELDS, frame );
            curl_easy_setopt( curl, CURLOPT_POSTFIELDSIZE, (long)post_len );
            curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );

            /* 发出请求并收完响应；失败则丢这一帧，继续听串口 */
            cres = curl_easy_perform( curl );
            if ( cres != CURLE_OK )
            {
                cerr = curl_easy_strerror( cres );
                app_warn( "%s: curl %s", object, cerr );
                fclose( fp );
                continue;
            }

            /* fwrite 可能还在缓冲里，先刷下去，再从头读出 body */
            fflush( fp );
            seek_ret = fseek( fp, 0, SEEK_SET );
            if ( seek_ret != 0 )
            {
                app_warning( "%s: fseek", object );
                fclose( fp );
                continue;
            }
            have = (int)fread( httpbuf, 1, sizeof(httpbuf), fp );
            fclose( fp );

            /* 把 HTTP 响应 body 原样写回串口（可能要 write 多次才写完） */
            app_info( "%s: HTTP body %d bytes -> serial", object, have );
            off = 0;
            while ( off < have )
            {
                nwrite = write( tty_fd, httpbuf + off, have - off );
                if ( nwrite < 0 )
                {
                    if ( errno == EINTR )
                    {
                        continue;
                    }
                    app_warning( "%s: serial write", object );
                    break;
                }
                if ( nwrite == 0 )
                {
                    break;
                }
                off += nwrite;
            }
        }
    }

    /* 服务退出前释放 curl 与串口；进程随后结束，框架不会走到更后面的清理 */
    if ( hdrs != NULL )
    {
        curl_slist_free_all( hdrs );
    }
    curl_easy_cleanup( curl );
    curl_global_cleanup();
    close( tty_fd );
    app_info( "%s: service exit", object );
    return tfalse;
}



/* HE 方法表：setup / shut / service；MAIN2API 生成 main */
static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "service", (comapi_t)_service },
};
MAIN2API( exe_api_table );
