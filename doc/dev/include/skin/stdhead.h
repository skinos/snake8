#ifndef  H_LAND_STDHEAD_H
#define  H_LAND_STDHEAD_H

/**
 * @file stdhead.h
 * @author dimmalex@gmail.com
 * @version 7.5
 * @date 20240903
 * @brief include the std header file
 */



/// include std libc header file
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <syslog.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/prctl.h>
#include <libgen.h>
#ifndef LAND_HAVE_LINUX_NETWORK
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#define LAND_HAVE_LINUX_NETWORK
#endif
#include <termios.h>
#include <pthread.h>
/// event loop header file
#include <event.h>
#include <event2/event.h>
#include <assert.h>



#endif   /* ----- #ifndef H_LAND_STDHEAD_H  ----- */

