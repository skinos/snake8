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
 * @brief get the persistent configuration from database
 * @param[in] com object pointer or string description (e.g., "land@machine")
 * @param[in] fa file attribute (namespace/file path), attr attribute path within the file
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
 * @brief get the configure value of the object from database
 * @param[out] buffer buffer to store value
 * @param[in] buflen buffer size
 * @param[in] com object pointer or string description (e.g., "land@machine")
 * @param[in] fa file attribute (namespace/file path), attr attribute path within the file
 * @return string for value or NULL
 *		@retval string for succeed
 *		@retval NULL for none value, the errno code maybe sets when error
 */
const char *dbs_fetch_string( char *buffer, int buflen,    obj_t com, attr_t fa, attr_t attr );
const char *dbs_fetchs_string( char *buffer, int buflen,   obj_t com, attr_t fa, const char *attr, ... );
const char *dbs_sfetchs_string( char *buffer, int buflen, const char *com, const char *fa, const char *attr, ... );



/**
 * @brief save configuration to persistent database
 * @param[in] com object pointer or string description (e.g., "land@machine")
 * @param[in] value json value to save
 * @param[in] attr attribute path within the file
 * @return the operation is succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, and errno code will be sets
 */
boole dbs_save( obj_t com, attr_t fa,                      talk_t value, attr_t attr );
boole dbs_saves( obj_t com, attr_t fa,                     talk_t v, const char *attr, ... );
boole dbs_ssave( const char *com, const char *fa,               talk_t value, attr_t attr );
boole dbs_ssaves( const char *com, const char *fa,             talk_t value, const char *attr, ... );
boole dbs_save_string( obj_t com, attr_t fa,               const char *value, attr_t attr );
boole dbs_ssave_string( const char *com, const char *fa, const char *value, attr_t attr );
boole dbs_ssaves_string( const char *com, const char *fa,const char *value, const char *attr, ... );



/**
 * @brief call a database table operation API on a component
 * @param[in] com object pointer identifying the component
 * @param[in] fa file attribute (namespace/file path) for the database
 * @param[in] api API method name to call on the database table (e.g., "list", "query")
 * @param[in] param parameter structure containing call arguments (NULL if not needed)
 * @return talk_t result from the table API
 *		@retval talk_t json for succeed - caller must free with talk_free()
 *		@retval NULL for failed, and errno code will be sets
 */
talk_t      dbs_table( obj_t com, attr_t fa, const char *api, param_t param );
/**
 * @brief get the database file pathname for a component
 * @param[out] buffer buffer to store the database file path
 * @param[in] buflen size of buffer
 * @param[in] com object pointer identifying the component
 * @param[in] fa file attribute (namespace/file path) for the database
 * @return string for database pathname
 *		@retval string (pointer to buffer) for succeed
 *		@retval NULL for failed, and errno code will be sets
 */
const char *dbs_path(     char *buffer, int buflen, obj_t com, attr_t fa );



#endif   /* ----- #ifndef H_LAND_CONFIG_H  ----- */

