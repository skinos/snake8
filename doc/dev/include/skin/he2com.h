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
 * @brief parse string he command into their corresponding structures
 * @param[in] string, he command
 * @return a pointer of he structure
 * 		@retval pointer for succeed
 *  	@retval NULL for failed, the errno code will be sets
 */
he_t        string2he( const char *cmd );
/**
 * @brief parse json he command into their corresponding structures
 * @param[in] cmd, he command in the json
 * @return a pointer of he structure
 * 		@retval he for succeed
 *  	@retval NULL for failed, the errno code will be sets
 */
he_t        json2he( talk_t cmd );
/**
 * @brief make json he command depend he structure
 * @param[in] h, a pointer of he structure
 * @return a pointer of he command json
 * 		@retval talk for succeed
 *  	@retval NULL for failed, the errno code will be sets
 */
talk_t      he2json( he_t h );
/**
 * @brief make string he command depend he structure
 * @param[in] h, a pointer of he structure
 * @return a string of he command json
 * 		@retval string for succeed
 *  	@retval NULL for failed, the errno code will be sets
 */
const char *he2string( he_t h );
/**
 * @brief free a he structure
 * @param[in] h a pointer of he structure
 * @return none
 */
void        he_free( he_t h );
/**
 * @brief execute the he command
 * @param[in] h, a pointer of he structure
 * @return a pointer of json
 *		@retval json for peer succeed
 *		@retval ttrue for peer return
 *		@retval tfalse for peer return, the errno code may be sets
 *		@retval NULL for peer failed, the errno code may be sets
 *  	@retval terror for peer got wrong or local error, the errno code will be sets
 *  	@retval tpanic for calling of parameter error, and errno code will be sets
 */
talk_t      he_execute( he_t h );



/**
 * @brief execute a string he command
 * @param[in] cmd, he command
 * @return a pointer of json
 *		@retval json for peer succeed
 *		@retval ttrue for peer return
 *		@retval tfalse for peer return, the errno code may be sets
 *		@retval NULL for peer failed, the errno code may be sets
 *  	@retval terror for peer got wrong or local error, the errno code will be sets
 *  	@retval tpanic for calling of parameter error, and errno code will be sets
 */
talk_t string_he_execute( const char *cmd );
/**
 * @brief execute a string he command at line command
 * @param[in] cmd, he command
 * @return succeed or failed, print the return at the terminal
 *		@retval 0 for succeed
 *		@retval other for failed
 */
int    line_he_command( const char *cmd );

/**
 * @brief execute a json he command
 * @param[in] cmd, json he command
 * @return json command result
 */
talk_t json_he_execute( talk_t cmd );
/**
 * @brief execute a talk that include list of he command
 * @param[in] cmd, list of he command
 * @return json command result
 */
talk_t talk_he_command( talk_t cmd );



#endif   /* ----- #ifndef H_LAND_HE2COM_H  ----- */

