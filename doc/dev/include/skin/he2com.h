#ifndef H_LAND_HE2COM_H
#define H_LAND_HE2COM_H

/**
 * @file he2com.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief The command line invokes the function implementation required by the component
 */



/* he command type */
#define HE_GET       0      /* get the configure value */
#define HE_SET       1      /* set the configure value */
#define HE_OR        2      /* modify the configure value */
#define HE_CALL      3      /* API call */
#define HE_DBS_GET   4      /* get the database value */
#define HE_DBS_SET   5      /* set the database value */
#define HE_DBS_OR    6      /* modify the database value */
#define HE_DBS_CALL  7      /* database API call */
/* he command structure */
typedef struct he_st
{
    /* he type */
    int flags;
    /* component */
    obj_t o;                                           // need free
    /* file attribute */
    attr_t f;                                          // need free
    /* method */
    char *m;
    /* parameter */
    param_t p;                                         // need free
    /* configure attribute */
    attr_t a;                                          // need free
    /* value */
    talk_t v;                                          // need free
    // buffer for command
    char *buffer;                                      // need free
    // command
    char *cmd;                                         // need free
} he_struct;
typedef he_struct* he_t;



/**
 * @brief parse main argv/argv into he structure (format: "project@component:attr=value" or "project@component.method[param]")
 * @param[in] argc main argc
 * @param[in] argv main argv
 * @return he structure pointer (must be freed with he_free)
 * 		@retval pointer for succeed
 *  	@retval NULL for failed, errno will be set
 */
he_t        argv2he( int argc, const char **argv );
/**
 * @brief parse he command string into he structure (format: "project@component:attr=value" or "project@component.method[param]")
 * @param[in] string he command string (e.g., "land@machine:wan/ip=192.168.1.1" or "land@machine.status[eth0]")
 * @return he structure pointer (must be freed with he_free)
 * 		@retval pointer for succeed
 *  	@retval NULL for failed, errno will be set
 */
he_t        string2he( const char *cmd );
/**
 * @brief parse json he command into their corresponding structures
 * @param[in] cmd he command as talk_t json
 * @return he structure pointer (must be freed with he_free)
 * 		@retval he for succeed
 *  	@retval NULL for failed, errno will be set
 */
he_t        json2he( talk_t cmd );
/**
 * @brief convert he structure to json he command representation
 * @param[in] h he structure pointer
 * @return talk_t json of he command
 * 		@retval talk_t for succeed - caller must free with talk_free()
 *  	@retval NULL for failed, errno will be set
 */
talk_t      he2json( he_t h );
/**
 * @brief convert he structure to string he command representation
 * @param[in] h he structure pointer
 * @return string of he command
 * 		@retval string for succeed (owned by he structure, valid until he_free)
 *  	@retval NULL for failed, errno will be set
 */
const char *he2string( he_t h );
/**
 * @brief free a he structure and all its members
 * @param[in] h he structure pointer to free
 * @return none
 */
void        he_free( he_t h );
/**
 * @brief execute the he command (HE_GET/HE_SET/HE_CALL/etc.)
 * @param[in] h he structure pointer (must be freed with he_free)
 * @return json result (must be freed with talk_free if > tpanic)
 *		@retval json for peer succeed
 *		@retval ttrue for peer return
 *		@retval tfalse for peer return, the errno code may be sets
 *		@retval NULL for peer failed, the errno code may be sets
 *  	@retval terror for peer got wrong or local error, errno will be set
 *  	@retval tpanic for calling of parameter error, and errno code will be sets
 */
talk_t      he_execute( he_t h );



/**
 * @brief execute a string he command
 * @param[in] cmd he command string
 * @return json result (must be freed with talk_free if > tpanic)
 *		@retval json for peer succeed
 *		@retval ttrue for peer return
 *		@retval tfalse for peer return, the errno code may be sets
 *		@retval NULL for peer failed, the errno code may be sets
 *  	@retval terror for peer got wrong or local error, errno will be set
 *  	@retval tpanic for calling of parameter error, and errno code will be sets
 */
talk_t string_he_execute( const char *cmd );
/**
 * @brief execute a string he command at line command
 * @param[in] cmd he command string
 * @return succeed or failed, print the return at the terminal
 *		@retval 0 for succeed
 *		@retval other for failed
 */
int    line_he_command( const char *cmd );

/**
 * @brief execute a json he command
 * @param[in] cmd he command as json (talk_t)
 * @return talk_t result from command execution
 * 		@retval talk_t json for succeed - caller must free with talk_free()
 *  	@retval ttrue for peer return true
 *  	@retval NULL for peer return NULL
 *  	@retval tfalse for peer return false
 *  	@retval terror for peer got wrong or local error, errno will be set
 *  	@retval tpanic for calling or parameter error, errno will be set
 */
talk_t json_he_execute( talk_t cmd );
/**
 * @brief execute a talk_t json array containing a list of he commands sequentially
 * @param[in] cmd json array where each element is a he command (json object)
 * @return talk_t result from the last command execution
 * 		@retval talk_t json for succeed - caller must free with talk_free()
 *  	@retval ttrue for peer return true
 *  	@retval NULL for peer return NULL
 *  	@retval tfalse for peer return false
 *  	@retval terror for peer got wrong or local error, errno will be set
 *  	@retval tpanic for calling or parameter error, errno will be set
 */
talk_t talk_he_command( talk_t cmd );



#endif   /* ----- #ifndef H_LAND_HE2COM_H  ----- */

