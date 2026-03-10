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
 * @brief char of string replace
 * @return none
 */
void char2char( char *src, char a, char b );
/**
 * @brief upper case the string
 * @return none
 */
void low2upp( char *str );
/**
 * @brief upper case the string
 * @return none
 */
void upp2low( char *str );

char *md5_encode( const char *s, int len );
char *b64_encode( const char *s, int len );
char *b64_decode( const char *s, int *len );

char *url_encode( char const *s, int len, int *new_length );
int   url_decode( char *str, int len );

char *simple_encode( const char *message, const char *key );
char *simple_decode( const char *message, const char *key );

void  string2hex( const char *src, char *dest, int len );
void  hex2string( const char *src, char *dest, int len);
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
boole		 string2mac( const char *macbuf, hp_mac_t mac );
boole		 mac2string( hp_mac_t mac, char *macbuf );
unsigned int mac2int( hp_mac_t mac );
boole		 mac2serial( hp_mac_t mac, char *macbuf );
void		 mac2add( hp_mac_t mac,  int i );
boole		 macrang( hp_mac_t mac,  hp_mac_t start, hp_mac_t end, int mod );



/**
 * @brief nothing to do of signal handler function
 * @param[in] signo signal number
 * @return none
 */
void		   signal_noprocess( int signo );
/**
 * @brief same the signal() function and more safe
 * @param[in] signo, signal number
 * @param[in] func, signal handler function
 * @param[in] sa_flags, sigaction flags( SA_RESTART )
 * @return original signal handler
 * 		@retval original signal handler for succeed
 *  	@retval SIG_ERR for failed, the errno code will be sets
 */
typedef void (*sighandler_t)(int);
sighandler_t   signal_register( int signo, sighandler_t func, int sa_flags );



/**
 * @brief count whole directory size, include the subdir
 * @param[in] dir, directory path
 * @return directory size
 *		@retval positive or zero for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         directory_size( const char *dir );
/**
 * @brief count directory size, not include the subdir
 * @param[in] dir, directory path
 * @return directory size
 *		@retval positive or zero for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         directory_subsize( const char *dir );          // Not Implemented
/**
 * @brief count how many file/dir in whole directory, include the subdir
 * @param[in] dir, directory path
 * @return how many file in the directory
 *		@retval positive for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int 		directory_sum( const char *dir );             // Not Implemented
/**
 * @brief count how many file in a directory
 * @param[in] dir, directory path
 * @return how many file in the directory
 *		@retval positive for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         directory_subsum( const char *dir );

/**
 * @brief lock the corresponding area of the file
 * @param[in] fd, file description handler
 * @param[in] ex, mutex when true
 * @param[in] start, starting offset for lock
 * @param[in] whence, How to interpret start: SEEK_SET, SEEK_CUR, SEEK_END
 * @param[in] len, number of bytes to lock
 * @param[in] wait, wait or not. -1 indicates permanent wait, 0 indicates no wait, and a positive integer indicates waiting time
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole       fd_lock( int fd, boole ex, int start, int whence, int len, int wait );
/**
 * @brief unlock the corresponding area of the file
 * @param[in] fd, file description handler
 * @param[in] ex, mutex when true
 * @param[in] start, starting offset for lock
 * @param[in] whence, How to interpret start: SEEK_SET, SEEK_CUR, SEEK_END
 * @param[in] len, number of bytes to lock
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole       fd_unlock( int fd, boole ex, int start, int whence, int len );
/**
 * @brief get the region locking pid of process corresponding to the file
 * @param[in] fd, file description handler
 * @param[in] ex, mutex when true
 * @param[in] start, starting offset for lock
 * @param[in] whence, how to interpret start: SEEK_SET, SEEK_CUR, SEEK_END
 * @param[in] len, number of bytes to lock
 * @return pid of process lock succeed
 *		@retval pid for succeed
 *		@retval negative for no lock or failed, the errno code will be sets
 */
pid_t       fd_lock_pid( int fd, boole ex, int start, int whence, int len );
/**
 * @brief set the fd block
 * @param[in] fd, file description handler
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole       fd_block( int fd );
/**
 * @brief set the fd nonblock
 * @param[in] fd, file description handler
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole       fd_nonblock( int fd );

/**
 * @brief open a file with a lock
 * @param[in] filename, file pathname
 * @param[in] flags, flags for open() function
 * @param[in] mode, mode for open() function
 * @param[in] block, block or not. -1 indicates permanent block, 0 indicates no block, and a positive integer indicates waiting time
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         lock_open( const char *filename, int flags, int mode, int block );
/**
 * @brief unlock and close the file
 * @param[in] fd, file description handler
 * @return opertion succeed or failed
 *		@retval 0 for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int         lock_close( int fd );

/**
 * @brief write a string to a file
 * @param[in] filename, file pathname
 * @param[in] format, string same printf()
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
 * @brief read a string to a file
 * @param[in] filename file pathname
 * @param[out] readbuf buffer the string will be store here
 * @param[in] bufsize size of buffer
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char *file2string( const char *filename, char *buffer, int bufsize );
/**
 * @brief write a number to a file
 * @param[in] filename file pathname
 * @param[in] number
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
 * @param[out] buffer, system uptime string store here
 * @param[in] buflen, buffer length
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char   *uptime_string( char *buffer, int buflen );
/**
 * @brief get the system uptime description
 * @param[out] buffer, system uptime description store here
 * @param[in] buflen, buffer length
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char   *uptime_desc( char *buffer, int buflen );
/**
 * @brief get the live time description
 * @param[in] ontime, uptime in the past point
 * @param[out] buffer, live time description store here
 * @param[in] buflen, buffer length
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char   *livetime_desc( unsigned int ontime, char *buffer, int buflen );
/**
 * @brief get the current date description
 * @param[out] buffer, current date description store here
 * @param[in] buflen, buffer length
 * @return string
 *		@retval string for succeed
 *		@retval NULL for failed, the errno code will be sets
 */
const char   *date_desc( char *buffer, int buflen );
/**
 * @brief set the current date
 * @param[in] seconds, current UTC second
 * @param[in] zone, zone
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole         date_set( time_t seconds, const char* zone );
/**
 * @brief adjust the current date by zone
 * @param[in] seconds, current UTC second
 * @param[in] zone, zone
 * @return time by zone
 */
time_t        date_adjust( time_t seconds, const char* zone );



/**
 * @brief same the system() function and more safe
 * @param[in] format, shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   shell( const char *format, ... );
/**
 * @brief same the system() function and more safe, also have timeout control
 * @param[in] timeout, return after timout
 * @param[in] silent, not to print when true
 * @param[in] format, shell command
 * @return return of command
 *		@retval 0 for succeed
 *		@retval negative for failed, the errno code will be sets
 */
int   execute( int timeout, boole silent, const char *format, ... );
#define silent_execute( ... )   execute( 0, 1, __VA_ARGS__ )
/**
 * @brief kill the pid force
 * @param[in] pid, process identify 
 * @param[in] timeout, timeout for second
 * @return operation succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole killpid( pid_t pid, int timeout );
/**
 * @brief exeute the ifconfig with lock
 * @param[in] format, shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   ifconfig( const char *format, ... );
/**
 * @brief exeute the iptables with lock
 * @param[in] format, shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   iptables( const char *format, ... );
/**
 * @brief exeute the ip6tables with lock
 * @param[in] format, shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   ip6tables( const char *format, ... );
/**
 * @brief exeute the ebtables with lock
 * @param[in] format, shell command
 * @return value of command return
 * 		@retval 0 for succeed
 *  	@retval negative for failed, the errno code will be sets
 */
int   ebtables( const char *format, ... );
/**
 * @brief insmod the module
 * @param[in] module, module pathname
 * @return none 
 */
int   insmod( const char *module );
/**
 * @brief rmmod the module
 * @param[in] module, module name
 * @return none 
 */
int   rmmod( const char *module );
/**
 * @brief found the module insmod already
 * @param[in] module, module name
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
 * @brief netmask transition
 */
const char  *ip2subnet( const char *ip, const char *mask, char *subnet, int len );
const char  *netmask2cidr( const char *netmask, char *buf, int buflen );
unsigned int netmask2num( const char *mask );
/**
 * @brief test the netcard have the flag
 * @param[in] card, network interface name
 * @param[in] flag, test the flag
 * @return flag is or not exist
 *		@retval 1 when flag exist
 *		@retval 0 when no exist
 *		@retval -1 for failed, the errno code will be sets
 */
int          netdev_flags( const char *card, short flag );
/**
 * @brief get the netcard basic infomation
 * @param[in] card, network interface name
 * @param[out] ip, network interface ip return to store here
 * @param[in] ip_len, ip length
 * @param[out] pppip, network interface ppp ip or gateway ip return to store here
 * @param[in] pppip_len, pppip length
 * @param[out] mask, network ineterface netmask return to store here
 * @param[in] mask_len, mask length
 * @param[out] mac, network interface mac address return to store here
 * @param[in] mac_len, mac length
 * @return opertion succeed or failed
 *		@retval 0 for succeed
 *		@retval -1 for failed, the errno code will be sets
 */
int          netdev_info( const char *card, char *ip, int ip_len, char *pppip, int pppip_len, char *mask, int mask_len, char *mac, int mac_len );
/**
 * @brief get the netcard flow infomation
 * @param[in] card, network interface name
 * @param[out] rx_bytes, save rx bytes
 * @param[out] rx_packets, save rx packets
 * @param[out] rx_errs, save rx error
 * @param[out] rx_drops, save rx drops
 * @param[out] tx_bytes, save tx bytes
 * @param[out] tx_packets, save tx packets
 * @param[out] tx_errs, save tx error
 * @param[out] tx_drops, save tx drops
 * @return opertion succeed or failed
 *		@retval 0 for succeed
 *		@retval -1 for failed, the errno code will be sets
 */
int          netdev_flew( const char *card, unsigned long long *rx_bytes , unsigned long long *rx_packets , unsigned long long *rx_errs, unsigned long long *rx_drops, unsigned long long *tx_bytes , unsigned long long *tx_packets , unsigned long long *tx_errs, unsigned long long *tx_drops );

/**
 * @brief get the route rule infomation
 * @param[in] destname, dest of rule
 * @param[in] mask, mask of rule
 * @param[in] metric, metric of rule, NULL for all metric
 * @param[out] gateway, save the rule gateway when rule exsit
 * @param[out] netdev, save the rule netdev when rule exsit
 * @return opertion succeed or failed
 *		@retval 0 for exist or succeed
 *		@retval -1 for failed, the errno code will be sets
 */
int          route_info( const char *destname, const char *mask, const char *metric, char *gateway, char *netdev );
/**
 * @brief get the extern route rule infomation
 * @param[in] tid, route table id
 * @param[in] destname, dest of rule
 * @param[in] mask, mask of rule
 * @param[in] metric, metric of rule, NULL for all metric
 * @param[out] gateway, save the rule gateway when rule exsit
 * @param[out] netdev, save the rule netdev when rule exsit
 * @return match the rule number
 *		@retval >0 for exist
 *		@retval 0 for not exist
 *		@retval <0 for error, the errno code will be sets
 */
int          routes_info( const char *tid, const char *destname, const char *mask, const char *metric, char *gateway, char *netdev );
/**
 * @brief get the default route infomation
 * @param[out] gateway
 * @param[out] netdev
 * @return rule exist or not
 *		@retval true for exist
 *		@retval false for not exist
 */
boole        outer_info( char *gateway, char *netdev );
/**
 * @brief get the gateway route infomation
 * @param[out] gateway
 * @param[out] netdev
 * @return rule exist or not
 *		@retval true for exist
 *		@retval false for not exist
 */
boole        gateway_info( char *gateway, char *netdev );
/**
 * @brief switch the extern route rule
 * @param[in] tid, route table id
 * @param[in] dest, dest ip address
 * @param[in] mask, mask of rule
 * @param[in] metric, metric of rule, NULL for all metric
 * @param[in] v, switch to the talk route infomation
 * @param[in] clear, true for delete the rule first
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        route_switch( const char *dest, const char *mask, const char *metric, talk_t v, boole clear );

/**
 * @brief switch the extern route rule
 * @param[in] tid route id
 * @param[in] dest dest ip address
 * @param[in] mask netmask
 * @param[in] metric
 * @param[in] v route infomation
 * @param[in] clear  true for delete first
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        routes_switch( const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, boole clear );
/**
 * @brief switch the extern route rule on two line
 * @param[in] tid route id
 * @param[in] dest dest ip address
 * @param[in] mask netmask
 * @param[in] metric
 * @param[in] v route infomation
 * @param[in] v2 route2 infomation
 * @param[in] clear  true for delete first
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole        routes_switch2( const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, talk_t v2, boole clear );
boole        routes_switch3( const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, talk_t v2, talk_t v3, boole clear );
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
 * @param[in] addr, domain name
 * @param[out] ipbuf, ip buffer
 * @param[in] ipbuflen, ip buffer length
 * @param[in] timeout, resolv timeout
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
 * @param[in] sock, file description handler
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_reuse( int sock );
/**
 * @brief set the socket no checksum
 * @param[in] sock, file description handler
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_nocheck( int sock );
/**
 * @brief set the fd block
 * @param[in] sock, file description handler
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_block( int sock );
/**
 * @brief set the fd nonblock
 * @param[in] sock, file description handler
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_nonblock( int sock );
/**
 * @brief set the tcp socket keepalive
 * @param[in] sock, file description handler
 * @param[in] keepintvl, socket keeplive interval
 * @param[in] keepidle, socket keeplive idle
 * @param[in] keepcnt, failed time count
 * @return opertion succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, the errno code will be sets
 */
boole socket_keepalive( int sock, int keepintvl, int keepidle, int keepcnt );
/**
 * @brief create tcp socket and connect to peer
 * @param[in] peer, only support ip address in string
 * @param[in] port, peer port
 * @param[in] timeout, connect timeout
 * @param[in] keepintvl, socket keeplive interval
 * @param[in] keepidle, socket keeplive idle
 * @param[in] keepcnt, failed time count
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval <0 for failed, the errno code will be sets
 */
int   tcp_connect( const char *peer, int port, int timeout, int keepintvl, int keepidle, int keepcnt );
/**
 * @brief create udp socket and connect to peer
 * @param[in] peer, only support ip address in string
 * @param[in] port, peer port
 * @param[in] timeout, connect timeout
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval <0 for failed, the errno code will be sets
 */
int   udp_connect( const char *peer, int port, int timeout );
/**
 * @brief create unix socket and connect to peer
 * @param[in] peer, unix domain path for peer
 * @param[in] local, local unix domain path
 * @param[in] type, unix type, SOCK_DGRAM or SOCK_STREAM
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval <0 for failed, the errno code will be sets
 */
int   unix_connect( const char *peer, const char *local, int type );
/**
 * @brief create unix socket for server
 * @param[in] local, local unix domain path
 * @param[in] type, unix type, SOCK_DGRAM or SOCK_STREAM
 * @return file description handler
 *		@retval >=0 for succeed
 *		@retval <0 for failed, the errno code will be sets
 */
int   unix_listen( const char *local, int type );

/**
 * @brief send a json/tnull/ttrue/tfalse/terror to file description handler or socket
 * @param[in] fd, file description handler or socket
 * @param[in] talk, a pointer of json or tnull/ttrue/tfalse/terror
 * @param[in] errocde, errno be send when json be tfalse/terror
 * @param[in] addr, use the sendto when not NULL
 * @param[in] addrlen, addr size
 * @param[in] timeout, <0 for block, >0 for wait timeout in second
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
 * @param[in] fd, file description handler or socket
 * @param[in] addr, use the recvfrom when not NULL
 * @param[in] addrlen, addr size
 * @param[in] timeout, <0 for block, >0 for wait timeout in second
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
 * @param[in] name, partition name
 * @param[out] mtd, save the char type mtd device pathname
 * @param[out] mtdblock, save the block type mtd device
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole         partition_dev( const char *name, char *mtd, char *mtdblock );
/**
 * @brief get the mmc device by partlabel name
 * @param[in] name, partlabel name
 * @param[out] mmc, save the char type mmc device pathname
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole         partlabel_dev( const char *name, char *mmc );
/**
 * @brief merge the adjust file to src in line by line, create the merge file
 * @param[in] gap, key and value gap
 * @param[in] src, srouce file
 * @param[in] adjust, this file merge
 * @param[in] merge, merge save this file
 * @return have merge operation
 * 		@retval true for merge 
 *  	@retval false for no merge, the errno code will be sets when error
 */
#define       MERGE_LINE_LEN 512
#define       MERGE_LINE_MAX 1024
boole         fileline_merge( const char *gap, const char *src, const char *adjust, const char *merge );



/******************************************************/
/******************************************************/
/******************************************************/
/*************** util_uart.c ************************/
/******************************************************/
/******************************************************/
/**
 * @brief open a uart
 * @param[in] path, uart pathname
 * @param[in] speed, uart speed( 9600, 115200 )
 * @param[in] parity, uart parity( 0 for none, 1 for odd, 2 for even )
 * @param[in] databit, uart databit( 5,6,7,8 )
 * @param[in] stopbit, uart stopbit( 1, 2 )
 * @param[in] flow, uart flow( 1 for soft, 2 for hard, 0 for none )
 * @return file desc handler
 * 		@retval >=0 for succeed
 *  	@retval <0 for failed, the errno code will be sets
 */
int uart_open( const char *path, int speed, int parity, int databit, int stopbit, int flow, int timeout );



#endif   /* ----- #ifndef H_LAND_UTILITY_H  ----- */

