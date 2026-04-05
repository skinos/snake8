#ifndef H_UART_LANDUART_H
#define H_UART_LANDUART_H
/**
 * @brief uart相关接口
 * @author dimmalex
 * @version 1.0
*/
#include <event.h>
#include <event2/event.h>



/**
 * @defgroup LogMacros tty Logging Macros
 * @brief Logging macro definitions based on interface name
 * @{
 */

/**
 * @brief Verbose level logging macro
 * @param[in] uart tty name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define uart_verbose( uart, ... )   uart_log( LANDLOG_VERBOSE, uart, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Debug level logging macro
 * @param[in] uart tty name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define uart_debug( uart, ... )     uart_log( LANDLOG_DEBUG, uart, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Info level logging macro
 * @param[in] uart tty name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define uart_info( uart, ... )      uart_log( LANDLOG_INFO, uart, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Warning level logging macro
 * @param[in] uart tty name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define uart_warn( uart, ... )      uart_log( LANDLOG_WARN, uart, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Warning level logging macro with errno
 * @param[in] uart tty name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define uart_warning( uart, ... )   uart_log( LANDLOG_WARN|LANDLOG_ERRNO, uart, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Fault level logging macro
 * @param[in] uart tty name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define uart_fault( uart, ... )     uart_log( LANDLOG_FAULT, uart, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Fault level logging macro with errno
 * @param[in] uart tty name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define uart_faulting( uart, ... )  uart_log( LANDLOG_FAULT|LANDLOG_ERRNO, uart, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Interface default logging function
 * @param[in] level Log level
 * @param[in] uart tty name
 * @param[in] file Source file name
 * @param[in] line Line number
 * @param[in] format Format string
 * @param[in] ... Variable arguments
 */
void uart_log( int level, const char *uart, const char *file, int line, const char *format, ... );



/* 判断是否为IO错误的宏*/
#define serial_error(num) ((num)==EIO)
/* Translate from bits/second to a speed_t. */
int serial_n2speed (int bps);
/* setdtr - control the DTR line on the serial port. This is called from die(), so it shouldn't call die(). */
void serial_setdtr (int tty_fd, int on);
/* serial open */
int serial_open( const char *path, talk_t cfg, int timeout );



#endif   /* ----- #ifndef H_UART_LANDUART_H  ----- */

