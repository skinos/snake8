#ifndef H_LAND_PARAM_H
#define H_LAND_PARAM_H
/**
 * @file param.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief implementation of parameter structure and related functions
 * @details used to pass strings or talk_t when called, component's function need accept a pointer of pararmter structure, strings and talk can build into paramter as options
 */



/// Maximum options in one paramter structure
#define PARAM_OPTIONS_MAX 20

/// parameter structure
/// example: 'myname,{"test":"testvalue"},test3' of options description for parameter
///    		 "myname" is first option, is a string option
///    		 '{"test":"testvalue"}' second option, is a json option
///    		 "test3" is third option, is a string option
typedef struct param_st
{
	// what serial number does the start with
    // example: offset is 0
	int start;

	// what serial number does the end with
    // example: size is 3
    int end;

    // string options pointer
    // example: id[0] point to "myname"                      param_string( obj, 1 );
    // 			id[1] point to '{"test":"testvalue"}'        param_string( obj, 2 );
    // 			id[2] point to "test3"                       param_string( obj, 3 );
    void *option[PARAM_OPTIONS_MAX];

    // talk options pointer
    // example: id[0] point to NULL
    // 			id[1] point to a talk of {"test":"testvalue"}
    // 			id[2] point to NULL
    talk_t talk[PARAM_OPTIONS_MAX];                                 // need free at param_free()

    // readable compiled string for this parameter
    // example: 'myname,{"test":"testvalue"},test3'
    char *string;                                                   // need free at param_free()

	// description the option is modifyied
	boole dirty;

	// description the option is pinter
	boole pointer;

} param_struct;
typedef param_struct* param_t;



/**
 * @brief create a structure of parameter from string of options description(dynamic allocation)
 * @param[in] string, description for options, copy to use
 * @return parameter
 * 		@retval parameter for succeed
 *  	@retval NULL for error, errno will be sets
 */
param_t     param_create( const char *a );
/**
 * @brief create a structure of parameter from json description(dynamic allocation)
 * @param[in] json, 1/2/3/4 attr will be dup as the options, dup to use
 * @return parameter
 * 		@retval parameter for succeed
 *  	@retval NULL for error, errno will be sets
 */
param_t     param_build( talk_t json );
/**
 * @brief create a structure of parameter from pointer(dynamic allocation)
 * @param[in] pointer, 1/2/3/4 attr will be reference as the options
 * @return parameter
 * 		@retval parameter for succeed
 *  	@retval NULL for error, errno will be sets
 */
param_t 	param_found( void *pointer, void *pointer2, void *pointer3, void *pointer4 );
/**
 * @brief free a parameter
 * @param[in] parameter
 * @return none
 */
void        param_free( param_t parameter );



/**
 * @brief create a structure of parameter and insert the option
 * @param[out] parameter, calloc a new paramter when this is NULL
 * @param[in] option, will insert to the parameter head
 * @return parameter
 * 		@retval parameter for succeed
 *  	@retval NULL for error, errno will be sets
 */
param_t 	param_inserts( param_t param, const char *option );
param_t 	param_insertt( param_t param, const talk_t option );
/**
 * @brief create a structure of parameter and add the option
 * @param[out] parameter, calloc a new paramter when this is NULL
 * @param[in] option, will add to the parameter end
 * @return parameter
 * 		@retval parameter for succeed
 *  	@retval NULL for error, errno will be sets
 */
param_t 	param_adds( param_t param, const char *option );
param_t 	param_addt( param_t param, const talk_t option );
/**
 * @brief adjust param beginning location, increase starting location
 * @param[in] parameter
 * @param[in] number, increase few starting location
 * @return opertion succeed or failed
 *		@retval 0 for succeed
 *		@retval negative for failed, the errno code will be sets
 */
boole       param_shift( param_t parameter, int number );
/**
 * @brief adjust param beginning location, decrease starting location
 * @param[in] parameter
 * @param[in] number, decrease few starting location
 * @return opertion succeed or failed
 *		@retval 0 for succeed
 *		@retval negative for failed, the errno code will be sets
 */
boole       param_unshift( param_t parameter, int number );
/**
 * @brief create a structure of parameter on json
 * @param[out] parameter, calloc a new paramter when this is NULL
 * @param[in] json, will be first and only option
 * @return parameter
 * 		@retval parameter for succeed
 *  	@retval NULL for error, errno will be sets
 */
param_t     param_import( param_t parameter, talk_t json );
/**
 * @brief create a structure of parameter on json
 * @param[out] parameter, calloc a new paramter when this is NULL
 * @param[in] json, will be first option
 * @param[in] json2, will be second option
 * @return parameter
 * 		@retval parameter for succeed
 *  	@retval NULL for error, errno will be sets
 */
param_t     param_import2( param_t parameter, talk_t json, talk_t json2 );



/**
 * @brief get how many options in parameter
 * @param[in] parameter
 * @return number of options
 * 		@retval positive or zeore for succeed
 *  	@retval negative for error, errno will be sets
 */
int         param_size( param_t parameter );
/**
 * @brief get string option of required from parameter, this value is also not available when the parameter is released
 * @param[in] parameter
 * @param[in] serial, serial number( It starts 1 ), -1 will for get the last option
 * @return string option
 * 		@retval string succeed
 *  	@retval NULL error, errno will be sets
 */
const char *param_string( param_t parameter, int serial );

/**
 * @brief get talk option of required from parameter, this value is also not available when the parameter is released
 * @param[in] parameter
 * @param[in] serial, serial number( It starts 1 ), -1 will for get the last option
 * @return talk option
 * 		@retval talk succeed
 *  	@retval NULL error, errno will be sets
 */
talk_t      param_talk( param_t parameter, int serial );

/**
 * @brief get pointer option of required from parameter, this value is also not available when the parameter is released
 * @param[in] parameter
 * @param[in] serial, serial number( It starts 1 ), -1 will for get the last option
 * @return pointer option
 * 		@retval pointer succeed
 *  	@retval NULL error, errno will be sets
 */
void       *param_pointer( param_t parameter, int serial );

/**
 * @brief get string of the options description with parameter, this value is also not available when the parameter is released
 * @param[in] parameter
 * @return string of the options description
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be sets
 */
const char *param_combine( param_t parameter );



#endif   /* ----- #ifndef H_LAND_PARAM_H  ----- */

