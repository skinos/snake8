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
 * @param[in] objp object pointer (e.g., obj_create("land@machine"))
 * @param[in] attr attribute path (e.g., attr_create("wan/ip"))
 * @return talk for value or NULL
 * @retval talk for value, must be freed with talk_free() after use
 * @retval NULL for none value, errno may be set when error
 * @retval tpanic for calling error, errno will be set
 * @note The returned talk_t is dynamically allocated and must be freed
 * @note Use config_gets/config_sgets for printf-style attribute paths
 * @see config_gets, config_sgets, config_get_string for string return
 * @note Difference from dbs_fetch: config_get reads config files, dbs_fetch reads the database layer
 * @note objp must represent project@component (obj_prj and obj_com both non-empty); otherwise EINVAL
 */
talk_t		config_get( obj_t objp, attr_t attr );
talk_t		config_gets( obj_t objp, const char *attr, ... );
talk_t		config_sget( const char *object, attr_t attr );
talk_t		config_sgets( const char *object, const char *attr, ... );
/**
 * @brief get the configure value of the object
 * @param[out] buffer buffer to store value
 * @param[in] buflen buffer size
 * @param[in] objp object pointer or string (e.g., "land@machine")
 * @param[in] attr attribute path (e.g., "wan/ip")
 * @return string for value or NULL
 *		@retval string for succeed
 *		@retval NULL for none value, the errno code maybe sets when error
 */
const char *config_get_string( char *buffer, int buflen,	obj_t objp, attr_t attr );
const char *config_gets_string( char *buffer, int buflen,  obj_t objp, const char *attr, ... );
const char *config_sgets_string( char *buffer, int buflen, const char *object, const char *attr, ... );



/**
 * @brief set the configuration value of the object
 * @param[in] objp object pointer (e.g., obj_create("land@machine"))
 * @param[in] v the value to set (talk_t/json type), will be copied internally
 * @param[in] attr attribute path (e.g., attr_create("wan/ip") or "network/config")
 * @return operation result
 * 		@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note This sets configuration
 * @note The value v is copied internally, caller retains ownership
 * @note objp must represent project@component (obj_prj and obj_com both non-empty); otherwise EINVAL
 * @note On success with unchanged content, returns true and sets errno to EEXIST (no file write)
 * @note On success with content written, returns true and sets errno to 0
 * @note Difference from dbs_save: config_set writes config files, dbs_save writes the database layer
 * @note Example:
 * @code
 * // Set configure IP address
 * obj_t obj = obj_create("network@eth0");
 * talk_t v = string2x("192.168.1.1");
 * attr_t a = attr_create("ip");
 * config_set(obj, v, a);
 * obj_free(obj);
 * talk_free(v);
 * attr_free(a);
 * @endcode
 * @see config_get for reading configuration
 * @see dbs_save for persistent storage
 */
boole config_set( obj_t objp,                     talk_t v, attr_t attr );
/** @note Same errno semantics as config_set (EEXIST if unchanged, 0 if written). */
boole config_sets( obj_t objp,                    talk_t v, const char *attr, ... );
/** @note Same errno semantics as config_set (EEXIST if unchanged, 0 if written). */
boole config_sset( const char *object,              talk_t v, attr_t attr );
/** @note Same errno semantics as config_set (EEXIST if unchanged, 0 if written). */
boole config_ssets( const char *object,             talk_t v, const char *attr, ... );
/** @note Same errno semantics as config_set (EEXIST if unchanged, 0 if written). */
boole config_set_string( obj_t objp,              const char *string, attr_t attr );
/** @note Same errno semantics as config_set (EEXIST if unchanged, 0 if written). */
boole config_sset_string( const char *object,  const char *string, attr_t attr );
/** @note Same errno semantics as config_set (EEXIST if unchanged, 0 if written). */
boole config_ssets_string( const char *object, const char *string, const char *attr, ... );



/**
 * @brief get the configure list of the object or entire system
 * @param[in] project project name (NULL for all projects)
 * @return talk for configure list
 *		@retval talk for succeed
 *		@retval NULL for failed, errno will be set
 */
talk_t      config_list(   const char *project );
/**
 * @brief get the configure pathname
 * @param[out] buffer buffer to store pathname
 * @param[in] buflen buffer size
 * @param[in] project project name
 * @param[in] filename configuration filename (NULL for directory)
 * @return string for configure pathname
 *		@retval string for succeed
 *		@retval NULL for failed, errno will be set
 */
const char *config_path(    char *buffer, int buflen, const char *project, const char *filename, ... );



#endif   /* ----- #ifndef H_LAND_CONFIG_H  ----- */

