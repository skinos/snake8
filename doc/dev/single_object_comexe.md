# 单实例可执行组件开发指南

## 概述

单实例可执行组件(Single-Instance Executable Component)是 SkinOS 框架中的一种独立进程组件,以 `PROJECT_ID@EXE_ID` 标识。与普通组件(component)不同,它编译为独立二进制文件,由框架通过 `execl` 启动,通过 Unix 域套接字保证同一时刻只有一个实例运行。

本文基于 `land`(核心框架)、`tmptools`(模板项目)和 `network@connect`(真实组件)三个项目总结开发模式。

## 与普通组件的区别

| 特性 | 普通组件(component) | 可执行组件(comexe) |
|------|-------------------|-------------------|
| 编译产物 | 共享库(.so),由 daemon dlopen 加载 | 独立二进制文件,由框架 execl 启动 |
| 入口 | dlsym 查找符号 | `MAIN2API` 宏生成 `main()` |
| 进程模型 | 作为 daemon 子进程 | 独立进程 |
| 适合场景 | 轻量级配置/查询服务 | 需要长期运行的守护进程、事件循环、复杂状态管理 |

## 目录结构

```
project/<project_name>/
├── Makefile              # OpenWrt 顶层包 Makefile
├── prj.json              # 项目清单(注册 exe/com/init/joint/wui)
└── <exe_id>/
    ├── <exe_id>.c        # 源码(含 API 实现 + API 表 + MAIN2API)
    └── mconfig           # 本地编译选项(CFLAGS/LDFLAGS)
```

示例:
```
project/network/
├── Makefile
├── prj.json
└── connect/
    ├── connect.c
    └── mconfig
```

## 构建系统

### exe.makefile 通用规则

可执行组件使用 `config/swrt5/exe.makefile`,关键机制:

1. **自动命名**: 目录名即二进制名 (`BINS := $(shell basename ${PWD_DIR})`)
2. **自动注入宏**:
   - `PROJECT_ID` — 来自父 Makefile
   - `COM_ID` / `EXE_ID` — 目录名
   - `COM_IDPATH` / `EXE_IDPATH` — `"PROJECT_ID@目录名"`
   - `gPLATFORM`, `gHARDWARE`, `gCUSTOM` 等平台宏
3. **本地 mconfig**: 通过 `sinclude mconfig` 引入,追加 CFLAGS/LDFLAGS

### mconfig 文件

```makefile
CFLAGS +=
LDFLAGS += -levent -lskinnet
```

根据需要链接外部库。常用:
- `-levent` — libevent 异步事件循环
- `-lskinnet` — 网络工具库
- `-lcrypto -lssl` — 加密库
- `-lreadline -lncurses` — 终端交互

## 核心编程模型

### 头文件

```c
#include "skin/skin.h"          // 框架主头文件(必须)
#include "skinnet/skinnet.h"    // 网络工具(按需)
```

`skin/skin.h` 自动引入所有框架头文件: `talk.h`(JSON 数据类型)、`com.h`(组件调用)、`config.h`(配置)、`register.h`(共享变量)、`log.h`(日志)、`serv.h`(服务管理)、`utility.h`(工具函数)等。

### API 函数签名

所有 API 函数统一签名:

```c
boole_t _my_api( obj_t this, param_t param );
```

- `this` — 当前对象(包含组件名等上下文)
- `param` — 参数集(通过 `param_string()`、`param_talk()` 等提取)
- 返回值: `ttrue`(成功), `tfalse`(失败), `terror`(错误), `tpanic`(严重错误), 或 `talk_t`(JSON 数据)

### API 调度表

定义静态表将字符串名映射到 C 函数:

```c
static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "service", (comapi_t)_service },
    { "status",  (comapi_t)_status },
    { "flush",   (comapi_t)_flush },
};
```

表的第一项为默认 API(未指定方法名时调用)。

### MAIN2API 宏

```c
MAIN2API( exe_api_table );
```

此宏展开为 `main()` 函数,实现:

1. 调用 `execute_object()` 判断是否由框架服务管理器启动
2. 若是,从环境变量获取 `obj_t`、`param_t`、`api` 名、pipe fd
3. 若否,调用 `argv2he(argc, argv)` 解析 HE 命令格式: `project@exe.method[param]`
4. 在 API 表中查找方法名,调用对应函数
5. 通过 pipe(服务模式)或 stdout(命令行模式)返回结果

## 生命周期函数

### _setup — 初始化

```c
boole_t _setup( obj_t this, param_t param )
{
    const char *object = obj_name( this );
    // 执行平台初始化脚本(可选)
    char path[PATH_MAX];
    if ( misc2path( path, sizeof(path), "myexe-setup.sh" ) != NULL )
    {
        shell( path );
    }
    // 注册并启动服务
    if ( cstart( this, "service", NULL, object ) == true )
    {
        return ttrue;
    }
    return tfalse;
}
```

`cstart()` 向服务管理器注册服务,若已运行则不重复启动。

### _shut — 关闭

```c
boole_t _shut( obj_t this, param_t param )
{
    const char *object = obj_name( this );
    sstop( object );    // 停止服务
    // 清理资源...
    return ttrue;
}
```

### _service — 服务主循环

这是长期运行的守护进程入口。典型结构:

```c
boole_t _service( obj_t this, param_t param )
{
    const char *object = obj_name( this );

    // 1. 加载配置
    talk_t cfg = config_get( this, NULL );

    // 2. 初始化 libevent
    struct event_config *econfig = event_config_new();
    event_config_require_features( econfig, EV_FEATURE_FDS );
    struct event_base *base = event_base_new_with_config( econfig );

    // 3. 注册信号处理
    struct event *ev_sigint = evsignal_new( base, SIGINT, my_exit, base );
    event_add( ev_sigint, NULL );
    struct event *ev_sigterm = evsignal_new( base, SIGTERM, my_exit, base );
    event_add( ev_sigterm, NULL );
    struct event *ev_sigpipe = evsignal_new( base, SIGPIPE, my_nodo, base );
    event_add( ev_sigpipe, NULL );

    // 4. 注册定时器
    struct event *ev_timer = event_new( base, -1, EV_TIMEOUT|EV_PERSIST, my_timer, base );
    struct timeval tv = { .tv_sec = 10, .tv_usec = 0 };
    evtimer_add( ev_timer, &tv );

    // 5. 创建 Unix 域套接字(单实例保证)
    char control_path[PATH_MAX];
    var2path( control_path, sizeof(control_path), "%s.unix", COM_ID );
    int sock = unix_listen( control_path, SOCK_DGRAM );
    struct event *ev_unix = NULL;
    if ( sock >= 0 )
    {
        fd_nonblock( sock );
        socket_reuse( sock );
        ev_unix = event_new( base, sock, EV_READ|EV_PERSIST, my_unix_handler, base );
        event_add( ev_unix, NULL );
    }

    // 6. 进入事件循环
    event_base_dispatch( base );

    // 7. 清理(退出时)
    if ( ev_unix != NULL ) { close( ev_unix->ev_fd ); event_free( ev_unix ); }
    event_free( ev_timer );
    // ... 释放其他 event ...
    event_base_free( base );
    event_config_free( econfig );
    talk_free( cfg );
    return tfalse;
}
```

## 单实例保证机制

### 1. 服务管理器层面

`cstart()` 向 SkinOS 服务管理器注册服务。服务管理器维护每个服务的 PID,重复调用 `cstart()` 不会启动第二个实例。

### 2. Unix 域套接字层面

```c
var2path( control_path, sizeof(control_path), "%s.unix", COM_ID );
sock = unix_listen( control_path, SOCK_DGRAM );
```

`unix_listen()` 内部调用 `bind()`。若套接字文件已存在(另一个实例正在运行),`bind()` 返回 `EADDRINUSE`,函数返回 -1。这是操作系统级别的互斥保证。

### 3. 信号刷新(不重启)

```c
boole_t _flush( obj_t this, param_t param )
{
    const char *object = obj_name( this );
    int pid = spid( object );       // 查询服务管理器获取 PID
    if ( pid > 0 )
    {
        kill( pid, SIGHUP );        // 发送刷新信号,不启动新实例
        return ttrue;
    }
    return tfalse;
}
```

## 控制接口(客户端)

其他组件或命令行可通过 Unix 域套接字与守护进程通信:

```c
talk_t myexe_call( const char *cmd, talk_t v, int timeout )
{
    int fd;
    talk_t ret;
    talk_t req;
    struct stat st;
    char path[PATH_MAX];
    char control[PATH_MAX];

    // 1. 检查守护进程是否运行
    project_var_path( control, sizeof(control), PROJECT_ID, "%s.unix", "myexe" );
    if ( stat( control, &st ) != 0 )
    {
        talk_free( v );
        errno = ENOENT;
        return tpanic;
    }

    // 2. 构造请求 JSON
    req = json_create( NULL );
    json_set_value( req, "v", v );
    json_set_string( req, "cmd", cmd );

    // 3. 连接并发送
    project_var_path( path, sizeof(path), PROJECT_ID, "%s.unix-%d", "myexe", getpid() );
    fd = unix_connect( control, path, SOCK_DGRAM );
    if ( fd < 0 ) { /* error */ }

    talk2udp( fd, req, 0, NULL, 0, timeout );
    ret = udp2talk( fd, NULL, NULL, timeout );

    close( fd );
    unlink( path );     // 清理临时本地套接字
    talk_free( req );
    return ret;
}
```

### 服务端接收处理

```c
void my_unix_handler( int fd, short what, void *arg )
{
    char request_string[JSON_LINE_MAX];
    struct sockaddr_un sockaddr;
    socklen_t socklen = sizeof(sockaddr);

    int nread = recvfrom( fd, request_string, sizeof(request_string)-1, 0,
                          (struct sockaddr*)&sockaddr, &socklen );
    if ( nread <= 0 ) return;
    request_string[nread] = '\0';

    talk_t r = string2json( request_string );
    talk_t reply = my_accept( json_string( r, "cmd" ), json_value( r, "v" ), arg );

    talk2udp( fd, reply, errno, (struct sockaddr*)&sockaddr, socklen, 0 );
    talk_free( r );
    talk_free( reply );
}
```

## 与其他组件交互

### 调用其他组件 API

```c
// 同步调用(阻塞等待结果)
talk_t result = scall( "project@component", "method", param );

// 格式化参数调用
scalls( "arch@gpio", "action", "network/online,%s", COM_IDPATH );
```

### 读取/写入配置

```c
// 读取配置
talk_t cfg = config_get( this, NULL );          // 读取全部
talk_t val = config_get( this, "attr_name" );   // 读取指定属性

// 写入配置
config_set( this, new_value, "attr_name" );
```

### 共享寄存器

```c
// 读取全局共享变量
const char *mode = reg_string( NULL, "network_mode" );
int value = reg_int( NULL, "my_counter" );

// 写入共享变量(可被其他进程读取)
reg_set_int( "my_counter", 42 );
reg_set_string( "my_status", "running" );
```

### 日志

```c
// 框架日志(带组件标识)
app_info( "%s: started", object );
app_warn( "%s: config missing", object );
app_error( "%s: fatal error", object );

// 网络模块日志(带接口名)
network_info( "switch gateway to %s", connection[right] );
network_warn( "unix_listen failed" );
network_fault( "%s exit", object );
```

### 路径工具

```c
char path[PATH_MAX];
project_var_path( path, sizeof(path), PROJECT_ID, "myfile" );
// => /var/<project_id>/myfile

var2path( path, sizeof(path), "%s.unix", COM_ID );
// => /var/.<com_id>.unix

misc2path( path, sizeof(path), "my-script.sh" );
// => 在 misc 路径中查找脚本
```

## prj.json 注册

在项目清单中注册可执行组件:

```json
{
    "name": "myproject",
    "type": "root",
    "version": "8.0.0",

    "exe": {
        "myexe": "description of my executable"
    },

    "init": {
        "app": {
            "myproject@myexe.setup": ""
        }
    },

    "joint": {
        "network/online": {
            "myproject@myexe.online": ""
        }
    }
}
```

- `exe` — 注册可执行组件(区别于 `com` 注册普通组件)
- `init` — 开机自启动(在指定级别调用 setup)
- `joint` — 订阅联合事件

## 从命令行调用

```bash
# HE 命令格式
he 'myproject@myexe.setup'
he 'myproject@myexe.shut'
he 'myproject@myexe.status'
he 'myproject@myexe.flush'

# 带参数
he 'myproject@myexe.myapi:param1,param2'
```

## 完整最小模板

```c
/*
 *  Description:  myexe — single-instance executable component
 *       Author:  yourname
 *      Company:  ASHYELF
 */

#include "skin/skin.h"

/* ---- 退出信号 ---- */
void my_exit( int fd, short what, void *arg )
{
    event_base_loopbreak( arg );
}
void my_nodo( int fd, short what, void *arg ){}

/* ---- 定时器 ---- */
void my_timer( int fd, short what, void *arg )
{
    /* periodic work here */
}

/* ---- Unix 控制接口 ---- */
void my_unix_handler( int fd, short what, void *arg )
{
    talk_t r, a;
    int nread;
    socklen_t socklen;
    struct sockaddr_un sockaddr;
    char request_string[JSON_LINE_MAX];

    do {
        socklen = sizeof(sockaddr);
        nread = recvfrom( fd, request_string, sizeof(request_string)-1, 0,
                          (struct sockaddr*)&sockaddr, &socklen );
        if ( nread <= 0 ) break;
        request_string[nread] = '\0';
        r = string2json( request_string );
        if ( r == NULL ) { a = tfalse; }
        else { a = json_create( NULL ); /* fill reply */ }
        talk2udp( fd, a, errno, (struct sockaddr*)&sockaddr, socklen, 0 );
        talk_free( r );
        talk_free( a );
    } while( nread >= 0 );
}

/* ---- 查询状态(客户端) ---- */
talk_t _status( obj_t this, param_t param )
{
    char control[PATH_MAX];
    project_var_path( control, sizeof(control), PROJECT_ID, "%s.unix", COM_ID );
    if ( stat( control, &(struct stat){0}) != 0 ) return tpanic;
    /* send request and return reply */
    return json_create( NULL );
}

/* ---- 刷新(发 SIGHUP 给运行中的实例) ---- */
boole_t _flush( obj_t this, param_t param )
{
    int pid = spid( obj_name( this ) );
    if ( pid > 0 ) { kill( pid, SIGHUP ); return ttrue; }
    return tfalse;
}

/* ---- 生命周期 ---- */
boole_t _setup( obj_t this, param_t param )
{
    char path[PATH_MAX];
    if ( misc2path( path, sizeof(path), "myexe-setup.sh" ) != NULL )
        shell( path );
    if ( cstart( this, "service", NULL, obj_name( this ) ) == true )
        return ttrue;
    return tfalse;
}

boole_t _shut( obj_t this, param_t param )
{
    sstop( obj_name( this ) );
    return ttrue;
}

boole_t _service( obj_t this, param_t param )
{
    struct event_base *base;
    struct event *ev_sigint, *ev_sigterm, *ev_sigpipe, *ev_timer, *ev_unix;
    struct timeval tv;
    char control_path[PATH_MAX];
    int sock;

    /* event base */
    struct event_config *econfig = event_config_new();
    event_config_require_features( econfig, EV_FEATURE_FDS );
    base = event_base_new_with_config( econfig );

    /* signals */
    ev_sigint  = evsignal_new( base, SIGINT,  my_exit, base ); event_add( ev_sigint, NULL );
    ev_sigterm = evsignal_new( base, SIGTERM, my_exit, base ); event_add( ev_sigterm, NULL );
    ev_sigpipe = evsignal_new( base, SIGPIPE, my_nodo, base ); event_add( ev_sigpipe, NULL );

    /* timer */
    ev_timer = event_new( base, -1, EV_TIMEOUT|EV_PERSIST, my_timer, base );
    tv.tv_sec = 10; tv.tv_usec = 0;
    evtimer_add( ev_timer, &tv );

    /* unix socket (single-instance guarantee) */
    var2path( control_path, sizeof(control_path), "%s.unix", COM_ID );
    sock = unix_listen( control_path, SOCK_DGRAM );
    ev_unix = NULL;
    if ( sock >= 0 )
    {
        fd_nonblock( sock );
        socket_reuse( sock );
        ev_unix = event_new( base, sock, EV_READ|EV_PERSIST, my_unix_handler, base );
        event_add( ev_unix, NULL );
    }

    /* enter event loop */
    event_base_dispatch( base );

    /* cleanup */
    if ( ev_unix != NULL ) { close( ev_unix->ev_fd ); event_free( ev_unix ); }
    event_free( ev_timer );
    event_free( ev_sigpipe );
    event_free( ev_sigterm );
    event_free( ev_sigint );
    event_base_free( base );
    event_config_free( econfig );
    return tfalse;
}

/* ---- API 调度表 ---- */
static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "service", (comapi_t)_service },
    { "status",  (comapi_t)_status },
    { "flush",   (comapi_t)_flush },
};
MAIN2API( exe_api_table );
```

## 开发步骤清单

1. **创建目录**: `project/<project>/<exe_id>/`
2. **编写源码**: 实现 `_setup`、`_shut`、`_service`,定义 `exe_api_table`,末尾 `MAIN2API( exe_api_table )`
3. **编写 mconfig**: 添加所需的 `LDFLAGS` 和 `CFLAGS`
4. **注册到 prj.json**: 在 `exe` 字段中添加条目
5. **(可选) init 注册**: 在 `init` 中添加 `project@exe.setup` 实现开机自启
6. **(可选) joint 订阅**: 在 `joint` 中添加事件订阅
7. **(可选) 客户端函数**: 编写 `xxx_call()` 供其他组件调用
8. **编译**: 在项目目录执行 `make`
9. **测试**: `he 'project@exe.setup'` 启动,`he 'project@exe.status'` 查询

## 参考项目

| 项目 | 路径 | 说明 |
|------|------|------|
| land | `project/land/` | 核心框架,skin 头文件、daemon、服务管理 |
| tmptools | `project/tmptools/` | 模板项目,`comexe/comexe.c` 为最简模板 |
| connect | `project/network/connect/` | 生产级示例:libevent 事件循环、Unix 控制接口、多连接管理 |
