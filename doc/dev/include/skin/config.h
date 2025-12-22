#ifndef H_LAND_CONFIG_H
#define H_LAND_CONFIG_H

/**
 * @file config.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief implementation function to get/set/list the config
 */



/**
 * @brief get the configure of the object
 * @param[in] com, a object pointer or string description for object
 * @param[in] attr, a attribute or string description for attribute
 * @return talk for value or NULL
 *		@retval talk for value
 *		@retval NULL for none value, the errno code maybe sets when error
 *		@retval tpanic for calling error, and errno code will be sets
 */
talk_t		config_get( obj_t com, attr_t attr );
talk_t		config_gets( obj_t com, const char *attr, ... );
talk_t		config_sget( const char *com, attr_t attr );
talk_t		config_sgets( const char *com, const char *attr, ... );
/**
 * @brief get the configure value of the object
 * @param[out] buffer, the value will be store here when succeed
 * @param[in] buflen, size of buffer
 * @param[in] com, a object pointer or string description for object
 * @param[in] attr, a attribute or string description for attribute
 * @return string for value or NULL
 *		@retval string for succeed
 *		@retval NULL for none value, the errno code maybe sets when error
 */
const char *config_get_string( char *buffer, int buflen,	obj_t com, attr_t attr );
const char *config_gets_string( char *buffer, int buflen,  obj_t com, const char *attr, ... );
const char *config_sgets_string( char *buffer, int buflen, const char *com, const char *attr, ... );



/**
 * @brief set the value of configure of the object
 * @param[in] com, a object pointer or string description for object
 * @param[in] value, a json or string
 * @param[in] attr, a attribute or string for description of which attribute
 * @return the operation is succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, and errno code will be sets
 */
boole config_set( obj_t com,                     talk_t v, attr_t attr );
boole config_sets( obj_t com,                    talk_t v, const char *attr, ... );
boole config_sset( const char *com,              talk_t v, attr_t attr );
boole config_ssets( const char *com,             talk_t v, const char *attr, ... );
boole config_set_string( obj_t com,              const char *string, attr_t attr );
boole config_sset_string( const char *com,  const char *string, attr_t attr );
boole config_ssets_string( const char *com, const char *string, const char *attr, ... );



/**
 * @brief get the configure list of the object or entire system
 * @param[in] project, project name( return configure list for entire system when NULL)
 * @return talk for configure list
 *		@retval talk for succeed
 *		@retval NULL for failed, and errno code will be sets
 */
talk_t      config_list(   const char *project );
/**
 * @brief get the configure pathname
 * @param[out] buffer, the configure pathname will be store here when succeed
 * @param[in] buflen, size of buffer
 * @param[in] project, project name
 * @param[in] filename, configure file name( return configure directory when NULL )
 * @return string for configure pathname
 *		@retval string for succeed
 *		@retval NULL for failed, and errno code will be sets
 */
const char *config_path(    char *buffer, int buflen, const char *project, const char *filename, ... );



#endif   /* ----- #ifndef H_LAND_CONFIG_H  ----- */

