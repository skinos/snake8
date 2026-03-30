#ifndef H_LAND_UTILITY_H
#define H_LAND_UTILITY_H

/**
 * @file utility.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief miscellaneous function implementation
 */



/**
 * @brief replace all occurrences of a character in a string (in-place)
 * @param[in] src source string to be modified
 * @param[in] a character to be replaced
 * @param[in] b character to replace with
 * @return none
 */
void char2char( char *src, char a, char b );
/**
 * @brief convert string to uppercase (in-place)
 * @param[in] str string to be modified (entire C string until '\\0', using toupper per byte)
 * @return none
 * @note Sets errno to EINVAL if str is NULL
 */
void low2upp( char *str );
/**
 * @brief convert string to lowercase (in-place)
 * @param[in] str string to be modified (entire C string until '\\0', using tolower per byte)
 * @return none
 * @note Sets errno to EINVAL if str is NULL
 */
void upp2low( char *str );

/**
 * @brief MD5 hash encoding
 * @param[in] s input string
 * @param[in] len input length (use strlen(s) if len <= 0)
 * @return MD5 hash string (32 hex characters + internal '\\0'), caller must free
 *   @retval string for succeed
 *   @retval NULL for failed (EINVAL if s is NULL; ENOMEM if OpenSSL MD context allocation fails)
 */
char *md5_encode( const char *s, int len );
/**
 * @brief Base64 encoding
 * @param[in] s input string
 * @param[in] len input length
 * @return Base64 encoded string, need to free after use
 *   @retval string for succeed
 *   @retval NULL for failed
 */
char *b64_encode( const char *s, int len );
/**
 * @brief Base64 decoding
 * @param[in] s Base64 encoded string
 * @param[out] len output length
 * @return Decoded string, need to free after use
 *   @retval string for succeed
 *   @retval NULL for failed
 */
char *b64_decode( const char *s, int *len );

/**
 * @brief URL encoding
 * @param[in] s input string
 * @param[in] len input length
 * @param[out] new_length output length
 * @return URL encoded string, need to free after use
 *   @retval string for succeed
 *   @retval NULL for failed
 */
char *url_encode( char const *s, int len, int *new_length );
/**
 * @brief URL decoding (in-place)
 * @param[in,out] str URL encoded string, decoded result stored in same buffer
 * @param[in] len string length
 * @return decoded length
 *   @retval >=0 for succeed
 *   @retval negative for failed
 */
int   url_decode( char *str, int len );

/**
 * @brief Lightweight AES-128-CBC encrypt + Base64 (default key snake8@SkinOS if tok NULL)
 * @param[in] message plaintext (non-empty)
 * @param[in] tok passphrase (at most 16 bytes used for key material; padded key buffer internally)
 * @return Base64 ciphertext, caller must free
 *   @retval string for succeed
 *   @retval NULL for failed (EINVAL empty message, or crypto/OOM)
 * @note Not a substitute for authenticated encryption; IV is zero-filled in implementation
 */
char *simple_encode( const char *message, const char *tok );
/**
 * @brief Inverse of simple_encode: Base64 decode + AES-128-CBC decrypt
 * @param[in] message Base64 blob, or plaintext if it starts with '*' (returns strdup of rest), or legacy "|*|V2" hex form
 * @param[in] tok same rules as simple_encode (NULL uses default)
 * @return plaintext, caller must free
 *   @retval string for succeed
 *   @retval NULL for failed
 */
char *simple_decode( const char *message, const char *tok );

/**
 * @brief Convert string to hexadecimal representation
 * @param[in] src source string
 * @param[out] dest destination buffer (2x length of src + 1)
 * @param[in] len source length
 * @return none
 */
void  string2hex( const char *src, char *dest, int len );
/**
 * @brief Convert hexadecimal string to normal string
 * @param[in] src hexadecimal string
 * @param[out] dest destination buffer (1/2 length of src + 1)
 * @param[in] len source length
 * @return none
 */
void  hex2string( const char *src, char *dest, int len);
/**
 * @brief Convert hexadecimal string to printf format (with 0x prefix)
 * @param[in] src hexadecimal string
 * @param[out] dest destination buffer
 * @param[in] len source length
 * @return none
 */
void  hex2printf( const char *src, char *dest, int len);

/* mac structure */
typedef struct hp_mac_st
{
	union
	{
		unsigned short one;
		unsigned char array[2];
	}head;	/* head two */
	union
	{
		unsigned int one;
		unsigned char array[4];
	}end;	/* end six */
} hp_mac_struct;
typedef hp_mac_struct* hp_mac_t;
/**
 * @brief Convert MAC address string to hp_mac_t structure
 * @param[in] macbuf MAC address string (e.g., "00:11:22:33:44:55")
 * @param[out] mac MAC address structure
 * @return operation succeed or failed
 *   @retval true for succeed
 *   @retval false for failed
 */
boole		 string2mac( const char *macbuf, hp_mac_t mac );
/**
 * @brief Convert hp_mac_t structure to MAC address string
 * @param[in] mac MAC address structure
 * @param[out] macbuf buffer to store MAC string (at least 18 bytes)
 * @return operation succeed or failed
 *   @retval true for succeed
 *   @retval false for failed
 */
boole		 mac2string( hp_mac_t mac, char *macbuf );
/**
 * @brief Convert MAC address to integer (last 4 bytes)
 * @param[in] mac MAC address structure
 * @return integer value of MAC address
 */
unsigned int mac2int( hp_mac_t mac );
/**
 * @brief Convert MAC address to serial number string
 * @param[in] mac MAC address structure
 * @param[out] macbuf buffer to store serial string
 * @return operation succeed or failed
 *   @retval true for succeed
 *   @retval false for failed
 */
boole		 mac2serial( hp_mac_t mac, char *macbuf );
/**
 * @brief Add offset to MAC address
 * @param[in,out] mac MAC address structure
 * @param[in] i offset to add
 * @return none
 */
void		 mac2add( hp_mac_t mac,  int i );
/**
 * @brief Check if MAC address is in range
 * @param[in] mac MAC address to check
 * @param[in] start start of MAC address range
 * @param[in] end end of MAC address range
 * @param[in] mod modulus for calculation
 * @return whether MAC is in range
 *   @retval true for in range
 *   @retval false for out of range
 */
boole		 macrang( hp_mac_t mac,  hp_mac_t start, hp_mac_t end, int mod );



/**
 * @brief nothing to do of signal handler function
 * @param[in] signo signal number
 * @return none
 */
void		   signal_noprocess( int signo );
/**
 * @brief same the signal() function and more safe
 * @param[in] signo signal number
 * @param[in] func signal handler function
 * @param[in] sa_flags sigaction flags (e.g., SA_RESTART)
 * @return original signal handler
 * 		@retval original signal handler for succeed
 *  	@retval SIG_ERR for failed, the errno code will be sets
 */
typedef void (*sighandler_t)(int);
sighandler_t   signal_register( int signo, sighandler_t func, int sa_flags );



/**
 * @brief count whole directory size, include the subdir
 * @param[in] dir directory path
 * @return directory size
 *		@retval positive or zero for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         directory_size( const char *dir );
/**
 * @brief count directory size, not include the subdir
 * @param[in] dir directory path
 * @warning Not implemented: there is no definition in this codebase. Do not call — link will fail with an undefined symbol.
 */
int         directory_subsize( const char *dir );
/**
 * @brief count how many file/dir in whole directory, include the subdir
 * @param[in] dir directory path
 * @warning Not implemented: there is no definition in this codebase. Do not call — link will fail with an undefined symbol.
 */
int 		directory_sum( const char *dir );
/**
 * @brief count how many file in a directory
 * @param[in] dir directory path
 * @return how many file in the directory
 *		@retval positive for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         directory_subsum( const char *dir );

/**
 * @brief lock the corresponding area of the file
 * @param[in] fd file descriptor
 * @param[in] ex exclusive lock if true
 * @param[in] start starting offset for lock
 * @param[in] whence seek reference (SEEK_SET, SEEK_CUR, SEEK_END)
 * @param[in] len number of bytes to lock
 * @param[in] wait wait mode: -1=forever, 0=no wait, >0=timeout in seconds
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole       fd_lock( int fd, boole ex, int start, int whence, int len, int wait );
/**
 * @brief unlock the corresponding area of the file
 * @param[in] fd file descriptor
 * @param[in] start starting offset for lock
 * @param[in] whence seek reference (SEEK_SET, SEEK_CUR, SEEK_END)
 * @param[in] len number of bytes to lock
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole       fd_unlock( int fd, int start, int whence, int len );
/**
 * @brief get the region locking pid of process corresponding to the file
 * @param[in] fd file descriptor
 * @param[in] ex exclusive lock if true
 * @param[in] start starting offset for lock
 * @param[in] whence seek reference (SEEK_SET, SEEK_CUR, SEEK_END)
 * @param[in] len number of bytes to lock
 * @return pid of process lock succeed
 *		@retval pid for succeed
 *		@retval negative for no lock or failed, the errno code will be sets
 */
pid_t       fd_lock_pid( int fd, boole ex, int start, int whence, int len );
/**
 * @brief set the fd block
 * @param[in] fd file descriptor
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole       fd_block( int fd );
/**
 * @brief set the fd nonblock
 * @param[in] fd file descriptor
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole       fd_nonblock( int fd );

/**
 * @brief open a file with a lock
 * @param[in] filename file pathname
 * @param[in] flags open flags (O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, etc.)
 * @param[in] mode file permissions mode (e.g., 0644)
 * @param[in] block block mode: -1=forever, 0=no block, >0=timeout seconds
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         lock_open( const char *filename, int flags, int mode, int block );
/**
 * @brief unlock and close the file
 * @param[in] fd file descriptor
 * @return operation succeed or failed
 *		@retval 0 for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         lock_close( int fd );

/**
 * @brief write a string to a file
 * @param[in] filename file pathname
 * @param[in] format printf-style format string
 * @return length of data written on success
 *		@retval positive for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         string2file( const char *filename, const char *format, ... );
/**
 * @brief write a string to a file end
 * @param[in] filename file pathname
 * @param[in] format string same printf()
 * @return length of data written on success
 *		@retval positive for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         string3file( const char *filename, const char *format, ... );
/**
 * @brief read a string from a file
 * @param[in] filename file pathname
 * @param[out] buffer buffer the string will be stored here
 * @param[in] bufsize size of buffer
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char *file2string( const char *filename, char *buffer, int bufsize );
/**
 * @brief write a number to a file
 * @param[in] filename file pathname
 * @param[in] number integer value to write
 * @return length of data written on success
 *		@retval positive for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         number2file( const char *filename, int number );
/**
 * @brief read a number from a file
 * @param[in] filename file pathname
 * @return number
 *		@retval integer for succeed
 *		@retval -1 for failed, the errno code will be sets
 */
int         file2number( const char *filename );



/**
 * @brief Get when timestamp, millisecond
 * @return success returns the current timestamp */
long long     time_stamp( void );
/**
 * @brief get system uptime in second
 * @return system uptime in second
 */
unsigned long uptime_int( void );
/**
 * @brief get the system uptime string
 * @param[out] buffer buffer to store uptime string
 * @param[in] buflen buffer length
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char   *uptime_string( char *buffer, int buflen );
/**
 * @brief get the system uptime description
 * @param[out] buffer buffer to store uptime description
 * @param[in] buflen buffer length
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char   *uptime_desc( char *buffer, int buflen );
/**
 * @brief get the live time description
 * @param[in] ontime timestamp in the past
 * @param[out] buffer buffer to store live time description
 * @param[in] buflen buffer length
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char   *livetime_desc( unsigned int ontime, char *buffer, int buflen );
/**
 * @brief get the current date description
 * @param[out] buffer buffer to store date description
 * @param[in] buflen buffer length
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char   *date_desc( char *buffer, int buflen );
/**
 * @brief set the current date
 * @param[in] seconds UTC timestamp
 * @param[in] zone timezone offset
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole         date_set( time_t seconds, const char* zone );
/**
 * @brief adjust the current date by zone
 * @param[in] seconds UTC timestamp
 * @param[in] zone timezone offset
 * @return time by zone
 */
time_t        date_adjust( time_t seconds, const char* zone );



/**
 * @brief Run a command via system(3) with extra validation (not a full shell sandbox)
 * @param[in] format printf-style format; result must fit in LINE_MAX-1
 * @return Same convention as system(3): -1 on fork/wait/signal error; otherwise wait status (use WIFEXITED/WEXITSTATUS)
 *		@retval -1 command rejected (EINVAL metacharacters/controls, ENOSPC overflow) or system() failure
 * @note Rejects common sh metacharacters (e.g. ; | < ` $() ${} and stray &) so behavior differs from raw system()
 * @note Allows typical redirects such as >/dev/null and 2>&1 (ampersand only immediately after '>')
 */
int   shell( const char *format, ... );
/**
 * @brief Run a program without shell: fork + execvp on whitespace-split argv
 * @param[in] timeout timeout in seconds (>0 enables SIGALRM and SIGKILL on expiry)
 * @param[in] silent if true, redirect stdin/stdout/stderr to /dev/null in child
 * @param[in] format printf-style single command line; split on ASCII space only (no quotes)
 * @return Child exit status (WEXITSTATUS) when wait succeeds and no library error; -1 on failure
 *		@retval -1 format error, fork/exec error, wait error, child stopped, or ETIMEDOUT/ECHILD/EINVAL as errno
 * @note At most 19 arguments after argv[0]; embedded spaces in arguments are not supported
 */
int   execute( int timeout, boole silent, const char *format, ... );
#define silent_execute( ... )   execute( 0, 1, __VA_ARGS__ )
/**
 * @brief kill the pid force
 * @param[in] pid process ID 
 * @param[in] timeout timeout in seconds
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole killpid( pid_t pid, int timeout );
/**
 * @brief execute the ifconfig with lock
 * @param[in] format shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   ifconfig( const char *format, ... );
/**
 * @brief execute the iptables with lock
 * @param[in] format shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   iptables( const char *format, ... );
/**
 * @brief execute the ip6tables with lock
 * @param[in] format shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   ip6tables( const char *format, ... );
/**
 * @brief execute the ebtables with lock
 * @param[in] format shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   ebtables( const char *format, ... );
/**
 * @brief Load kernel module via modprobe if not already in /proc/modules
 * @param[in] module module name passed to modprobe
 * @return 0 on success, -1 on error (errno set; EEXIST if already loaded)
 */
int   insmod( const char *module );
/**
 * @brief Unload kernel module via rmmod if listed in /proc/modules
 * @param[in] module module name
 * @return 0 on success, -1 on error (errno set; EINVAL if module not loaded)
 */
int   rmmod( const char *module );
/**
 * @brief found the module insmod already
 * @param[in] module module name
 * @return true for insmod already, false for not found 
 */
boole lsmod( const char *module );



/******************************************************/
/******************************************************/
/******************************************************/
/*************** util_network.c ***********************/
/******************************************************/
/******************************************************/
#define MAIN_TABLE_NAME       "main"
#define MAIN_TABLE_ID         254
#define MAIN_TABLE_PREF       32766
#define IFNAME_TABLE_PREF     35000
#define RULE_TABLE_PREF       40000
#define DEFAULT_TABLE_NAME    "default"
#define DEFAULT_TABLE_ID      253
#define DEFAULT_TABLE_PREF    50000
#define EXTERN_METRIC         "10"
#define VPN_METRIC            "5"
/**
 * @brief calculate the subnet address from IP and netmask
 * @param[in] ip IP address string (e.g., "192.168.1.100")
 * @param[in] mask netmask string (e.g., "255.255.255.0")
 * @param[out] subnet buffer to store the subnet address (e.g., "192.168.1.0")
 * @param[in] len buffer size
 * @return subnet address string
 * 		@retval string for succeed (pointer to subnet buffer)
 *  	@retval NULL for failed
 */
const char  *ip2subnet( const char *ip, const char *mask, char *subnet, int len );
/**
 * @brief convert dotted netmask to CIDR prefix length string
 * @param[in] netmask dotted netmask string (e.g., "255.255.255.0")
 * @param[out] buf buffer to store CIDR string (e.g., "24")
 * @param[in] buflen buffer size
 * @return CIDR prefix length string
 * 		@retval string for succeed (pointer to buf)
 *  	@retval NULL for failed
 */
const char  *netmask2cidr( const char *netmask, char *buf, int buflen );
/**
 * @brief convert dotted netmask string to numeric prefix length
 * @param[in] mask dotted netmask string (e.g., "255.255.255.0")
 * @return CIDR prefix length as unsigned integer (e.g., 24)
 */
unsigned int netmask2num( const char *mask );
/**
 * @brief test the netcard have the flag
 * @param[in] card network interface name
 * @param[in] flag flag to test
 * @return flag is or not exist
 *		@retval 1 when flag exist
 *		@retval 0 when no exist
 *		@retval -1 for failed, the errno code will be sets
 */
int          netdev_flags( const char *card, short flag );
/**
 * @brief get the netcard basic information
 * @param[in] card network interface name
 * @param[out] ip buffer to store IP address
 * @param[in] ip_len IP address buffer length
 * @param[out] pppip buffer to store PPP/gateway IP
 * @param[in] pppip_len PPPoE IP buffer length
 * @param[out] mask buffer to store netmask
 * @param[in] mask_len netmask buffer length
 * @param[out] mac buffer to store MAC address
 * @param[in] mac_len MAC address buffer length
 * @return operation succeed or failed
 *		@retval 0 for succeed
 *		@retval -1 for failed, the errno code will be sets
 */
int          netdev_info( const char *card, char *ip, int ip_len, char *pppip, int pppip_len, char *mask, int mask_len, char *mac, int mac_len );
/**
 * @brief get the netcard flow information
 * @param[in] card network interface name
 * @param[out] rx_bytes received bytes
 * @param[out] rx_packets received packets
 * @param[out] rx_errors receive errors
 * @param[out] rx_drops receive drops
 * @param[out] tx_bytes transmitted bytes
 * @param[out] tx_packets transmitted packets
 * @param[out] tx_errors transmit errors
 * @param[out] tx_drops transmit drops
 * @return operation succeed or failed
 *		@retval 0 for succeed
 *		@retval -1 for failed, the errno code will be sets
 */
int          netdev_flew( const char *card, unsigned long long *rx_bytes , unsigned long long *rx_packets , unsigned long long *rx_errs, unsigned long long *rx_drops, unsigned long long *tx_bytes , unsigned long long *tx_packets , unsigned long long *tx_errs, unsigned long long *tx_drops );

/**
 * @brief get the route rule information
 * @param[in] destname destination address
 * @param[in] mask network mask
 * @param[in] metric route metric (NULL for all)
 * @param[out] gateway buffer to store gateway IP
 * @param[out] netdev buffer to store network device name
 * @return operation succeed or failed
 *		@retval 0 for exist or succeed
 *		@retval -1 for failed, the errno code will be sets
 */
int          route_info( const char *destname, const char *mask, const char *metric, char *gateway, char *netdev );
/**
 * @brief get the extern route rule information
 * @param[in] tid routing table ID
 * @param[in] destname destination address
 * @param[in] mask network mask
 * @param[in] metric route metric (NULL for all)
 * @param[out] gateway buffer to store gateway IP
 * @param[out] netdev buffer to store network device name
 * @return match the rule number
 *		@retval >0 for exist
 *		@retval 0 for not exist
 *		@retval <0 for error, the errno code will be sets
 */
int          routes_info( const char *tid, const char *destname, const char *mask, const char *metric, char *gateway, char *netdev );
/**
 * @brief get the default route information
 * @param[out] gateway buffer to store default gateway IP address
 * @param[out] netdev buffer to store default network device name
 * @return rule exist or not
 *		@retval true for exist
 *		@retval false for not exist
 */
boole        outer_info( char *gateway, char *netdev );
/**
 * @brief get the gateway route information
 * @param[out] gateway buffer to store gateway IP address
 * @param[out] netdev buffer to store network device name
 * @return rule exist or not
 *		@retval true for exist
 *		@retval false for not exist
 */
boole        gateway_info( char *gateway, char *netdev );
/**
 * @brief switch the default route rule in the main routing table
 * @param[in] dest destination IP address (e.g., "0.0.0.0" for default)
 * @param[in] mask network mask (e.g., "0.0.0.0")
 * @param[in] metric route metric (NULL for all)
 * @param[in] v talk_t containing route information (gateway, device, etc.)
 * @param[in] clear delete existing rule first if true
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        route_switch( const char *dest, const char *mask, const char *metric, talk_t v, boole clear );

/**
 * @brief switch the extern route rule in a specified routing table
 * @param[in] tid routing table ID string
 * @param[in] dest destination IP address
 * @param[in] mask network mask
 * @param[in] metric route metric (NULL for all)
 * @param[in] v talk_t containing route information (gateway, device, etc.)
 * @param[in] clear delete existing rule first if true
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        routes_switch( const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, boole clear );
/**
 * @brief switch the extern route rule with dual-path (two routes for load balancing/failover)
 * @param[in] tid routing table ID string
 * @param[in] dest destination IP address
 * @param[in] mask network mask
 * @param[in] metric route metric (NULL for all)
 * @param[in] v talk_t containing first route information
 * @param[in] v2 talk_t containing second route information
 * @param[in] clear delete existing rule first if true
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        routes_switch2( const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, talk_t v2, boole clear );
/**
 * @brief switch the extern route rule with triple-path (three routes for load balancing/failover)
 * @param[in] tid routing table ID string
 * @param[in] dest destination IP address
 * @param[in] mask network mask
 * @param[in] metric route metric (NULL for all)
 * @param[in] v talk_t containing first route information
 * @param[in] v2 talk_t containing second route information
 * @param[in] v3 talk_t containing third route information
 * @param[in] clear delete existing rule first if true
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        routes_switch3( const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, talk_t v2, talk_t v3, boole clear );
/**
 * @brief switch the extern route rule with quad-path (four routes for load balancing/failover)
 * @param[in] tid routing table ID string
 * @param[in] dest destination IP address
 * @param[in] mask network mask
 * @param[in] metric route metric (NULL for all)
 * @param[in] v talk_t containing first route information
 * @param[in] v2 talk_t containing second route information
 * @param[in] v3 talk_t containing third route information
 * @param[in] v4 talk_t containing fourth route information
 * @param[in] clear delete existing rule first if true
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        routes_switch4( const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, talk_t v2, talk_t v3, talk_t v4, boole clear );
/**
 * @brief create the extern route table for ifname
 * @param[in] tid route id
 * @param[in] ifnamest ifname status
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        routes_ifname( int tid, talk_t ifnamest );

/**
 * @brief resolv the domain to ip
 * @param[in] addr domain name to resolve
 * @param[out] ipbuf buffer to store resolved IP
 * @param[in] ipbuflen IP buffer length
 * @param[in] timeout DNS resolution timeout
 * @return ip address
 *		@retval ip address
 *		@retval NULL for failed, the errno code will be sets
 */
const char *domain2ip( const char *addr, char *ipbuf, int ipbuflen, int timeout );



/******************************************************/
/******************************************************/
/******************************************************/
/*************** util_socket.c ************************/
/******************************************************/
/******************************************************/
/**
 * @brief set the socket reuse
 * @param[in] sock socket file descriptor
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_reuse( int sock );
/**
 * @brief set the socket no checksum
 * @param[in] sock socket file descriptor
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_nocheck( int sock );
/**
 * @brief set the fd block
 * @param[in] sock socket file descriptor
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_block( int sock );
/**
 * @brief set the fd nonblock
 * @param[in] sock socket file descriptor
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_nonblock( int sock );
/**
 * @brief set the tcp socket keepalive
 * @param[in] sock socket file descriptor
 * @param[in] keepintvl keepalive interval seconds
 * @param[in] keepidle keepalive idle seconds
 * @param[in] keepcnt failed probe count
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_keepalive( int sock, int keepintvl, int keepidle, int keepcnt );
/**
 * @brief create tcp socket and connect to peer
 * @param[in] peer peer IP address string
 * @param[in] port peer port number
 * @param[in] timeout connection timeout
 * @param[in] keepintvl keepalive interval seconds
 * @param[in] keepidle keepalive idle seconds
 * @param[in] keepcnt failed probe count
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval <0 for failed, the errno code will be sets
 */
int   tcp_connect( const char *peer, int port, int timeout, int keepintvl, int keepidle, int keepcnt );
/**
 * @brief create udp socket and connect to peer
 * @param[in] peer peer IP address string
 * @param[in] port peer port number
 * @param[in] timeout connection timeout
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval <0 for failed, the errno code will be sets
 */
int   udp_connect( const char *peer, int port, int timeout );
/**
 * @brief create unix socket and connect to peer
 * @param[in] peer peer unix domain socket path
 * @param[in] local local unix domain socket path
 * @param[in] type socket type (SOCK_DGRAM or SOCK_STREAM)
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval <0 for failed, the errno code will be sets
 */
int   unix_connect( const char *peer, const char *local, int type );
/**
 * @brief create unix socket for server
 * @param[in] local local unix domain socket path
 * @param[in] type socket type (SOCK_DGRAM or SOCK_STREAM)
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval <0 for failed, the errno code will be sets
 */
int   unix_listen( const char *local, int type );

/**
 * @brief send a json/tnull/ttrue/tfalse/terror to file description handler or socket
 * @param[in] fd file descriptor or socket
 * @param[in] talk talk_t data to send
 * @param[in] errocde error code to send when talk is tfalse/terror
 * @param[in] addr destination address (NULL for connected sockets)
 * @param[in] addrlen address structure size
 * @param[in] timeout timeout: <0=block forever, >0=seconds
 * @return number of send bytes
 *		@retval >0 for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int 		talk2fd( int fd, talk_t talk, int errcode );
int         talk2tcp( int fd, talk_t talk, int errcode, int timeout );
int         talk2udp( int fd, talk_t talk, int errcode, struct sockaddr *addr, int addrlen, int timeout );
int         talk2socket( int fd, talk_t talk, int errcode, struct sockaddr *addr, int addrlen, int timeout );
/**
 * @brief read a json/tnull/ttrue/tfalse/terror from file description handler or socket
 * @param[in] fd file descriptor or socket
 * @param[in] addr source address (NULL if not needed)
 * @param[in] addrlen address structure size
 * @param[in] timeout timeout: <0=block forever, >0=seconds
 * @return a pointer of json/tnull/ttrue/tfalse/terror
 *		@retval json for peer succeed
 *		@retval NULL for peer return
 *		@retval ttrue for peer return
 *		@retval tfalse for peer return
 *  	@retval terror for peer got wrong, the errno code will be sets
 *  	@retval tpanic for operation got wrong, the errno code will be sets
 */
talk_t		fd2talk( int fd );
talk_t      tcp2talk( int fd, int timeout );
talk_t      udp2talk( int fd, struct sockaddr *addr, socklen_t *addrlen, int timeout );
talk_t      socket2talk( int fd, struct sockaddr *addr, socklen_t *addrlen, int timeout );



/******************************************************/
/******************************************************/
/******************************************************/
/*************** util_system.c ************************/
/******************************************************/
/******************************************************/
/**
 * @brief get the random
 * @return random
 */
unsigned long random_long( void );
/**
 * @brief get the mtd device by partition name
 * @param[in] name partition name
 * @param[out] mtd buffer to store MTD character device path
 * @param[out] mtdblock buffer to store MTD block device path
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole         partition_dev( const char *name, char *mtd, char *mtdblock );
/**
 * @brief get the mmc device by partlabel name
 * @param[in] name partition label name
 * @param[out] mmc buffer to store MMC device path
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole         partlabel_dev( const char *name, char *mmc );
/**
 * @brief merge the adjust file to src in line by line, create the merge file
 * @param[in] gap key-value separator string
 * @param[in] src source file path
 * @param[in] adjust adjustment file path
 * @param[in] merge merge output file path
 * @return have merge operation
 * 		@retval true for merge 
 *  	@retval false for no merge, the errno code will be sets when error
 */
#define       MERGE_LINE_LEN 512
#define       MERGE_LINE_MAX 1024
boole         fileline_merge( const char *gap, const char *src, const char *adjust, const char *merge );
/**
 * @brief check shell command string for injection characters
 * @param[in] cmd command string to check
 * @return whether injection characters were found
 * 		@retval true for injection detected
 *  	@retval false for safe command
 */
boole         shell_injection_check( const char *cmd );



/******************************************************/
/******************************************************/
/******************************************************/
/*************** util_uart.c ************************/
/******************************************************/
/******************************************************/
/**
 * @brief open a uart
 * @param[in] path UART device path
 * @param[in] speed baud rate (9600, 115200, etc.)
 * @param[in] parity parity (0=none, 1=odd, 2=even)
 * @param[in] databit data bits (5, 6, 7, 8)
 * @param[in] stopbit stop bits (1 or 2)
 * @param[in] flow flow control (0=none, 1=software, 2=hardware)
 * @param[in] timeout read timeout in deciseconds (tenths of a second), 0 for non-blocking
 * @return file desc handler
 * 		@retval >=0 for succeed
 *  	@retval <0 for failed, the errno code will be sets
 */
int uart_open( const char *path, int speed, int parity, int databit, int stopbit, int flow, int timeout );



#endif   /* ----- #ifndef H_LAND_UTILITY_H  ----- */

