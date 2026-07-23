#ifndef H_GNSS_SKINGNSS_H
#define H_GNSS_SKINGNSS_H
/**
 * @brief gnss related interfaces
 * @author dimmalex
 * @version 1.0
*/
#include <event.h>
#include <event2/event.h>



/**
 * @defgroup LogMacros gnss Logging Macros
 * @brief Logging macro definitions based on instance name
 * @{
 */

/**
 * @brief Verbose level logging macro
 * @param[in] gnss instance name (e.g. nmea / nmea2)
 * @param[in] ... Variable arguments, format string and parameters
 */
#define gnss_verbose( gnss, ... )   gnss_log( LANDLOG_VERBOSE, gnss, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Debug level logging macro
 * @param[in] gnss instance name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define gnss_debug( gnss, ... )     gnss_log( LANDLOG_DEBUG, gnss, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Info level logging macro
 * @param[in] gnss instance name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define gnss_info( gnss, ... )      gnss_log( LANDLOG_INFO, gnss, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Warning level logging macro
 * @param[in] gnss instance name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define gnss_warn( gnss, ... )      gnss_log( LANDLOG_WARN, gnss, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Warning level logging macro with errno
 * @param[in] gnss instance name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define gnss_warning( gnss, ... )   gnss_log( LANDLOG_WARN|LANDLOG_ERRNO, gnss, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Fault level logging macro
 * @param[in] gnss instance name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define gnss_fault( gnss, ... )     gnss_log( LANDLOG_FAULT, gnss, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Fault level logging macro with errno
 * @param[in] gnss instance name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define gnss_faulting( gnss, ... )  gnss_log( LANDLOG_FAULT|LANDLOG_ERRNO, gnss, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Instance default logging function
 * @param[in] level Log level
 * @param[in] gnss instance name
 * @param[in] file Source file name
 * @param[in] line Line number
 * @param[in] format Format string
 * @param[in] ... Variable arguments
 */
void gnss_log( int level, const char *gnss, const char *file, int line, const char *format, ... );



/* Check whether errno is an IO error */
#define serial_error(num) ((num)==EIO)
/* Translate from bits/second to a speed_t. */
int serial_n2speed (int bps);
/* setdtr - control the DTR line on the serial port. This is called from die(), so it shouldn't call die(). */
void serial_setdtr (int tty_fd, int on);
/* serial open */
int serial_open( const char *path, talk_t cfg, int timeout );



#endif   /* ----- #ifndef H_GNSS_SKINGNSS_H  ----- */
