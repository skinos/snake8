#ifndef H_LAND_DBS_H
#define H_LAND_DBS_H

/**
 * @file dbs.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief implementation function to fetch/save/list the config
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
talk_t		dbs_fetch( obj_t com, attr_t fa, attr_t attr );
talk_t		dbs_fetchs( obj_t com, attr_t fa, const char *attr, ... );
talk_t		dbs_sfetch( const char *com, const char *fa, attr_t attr );
talk_t		dbs_sfetchs( const char *com, const char *fa, const char *attr, ... );
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
const char *dbs_fetch_string( char *buffer, int buflen,    obj_t com, attr_t fa, attr_t attr );
const char *dbs_fetchs_string( char *buffer, int buflen,   obj_t com, attr_t fa, const char *attr, ... );
const char *dbs_sfetchs_string( char *buffer, int buflen, const char *com, const char *fa, const char *attr, ... );



/**
 * @brief save the value of db of the object
 * @param[in] com, a object pointer or string description for object
 * @param[in] value, a json or string
 * @param[in] attr, a attribute or string for description of which attribute
 * @return the operation is succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, and errno code will be sets
 */
boole dbs_save( obj_t com, attr_t fa,                      talk_t value, attr_t attr );
boole dbs_saves( obj_t com, attr_t fa,                     talk_t v, const char *attr, ... );
boole dbs_ssave( const char *com, const char *fa,               talk_t value, attr_t attr );
boole dbs_sssaves( const char *com, const char *fa,             talk_t value, const char *attr, ... );
boole dbs_save_string( obj_t com, attr_t fa,               const char *value, attr_t attr );
boole dbs_ssave_string( const char *com, const char *fa, const char *value, attr_t attr );
boole dbs_ssaves_string( const char *com, const char *fa,const char *value, const char *attr, ... );



/**
 * @brief get the configure list of the object or entire system
 * @param[in] project, project name( return configure list for entire system when NULL)
 * @return talk for configure list
 *		@retval talk for succeed
 *		@retval NULL for failed, and errno code will be sets
 */
talk_t      dbs_table( obj_t com, attr_t fa, const char *api, param_t param );
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
const char *dbs_path(     char *buffer, int buflen, obj_t com, attr_t fa );



#endif   /* ----- #ifndef H_LAND_CONFIG_H  ----- */

