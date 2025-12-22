#ifndef H_LAND_TALK_H
#define H_LAND_TALK_H

/**
 * @file talk.h
 * @author dimmalex@gmail.com
 * @version 7.5
 * @date 20240903
 * @brief Implementation of common communication data types
 * @details common communication data types named talk_t, Used for information transfer between components, Used by configuration information, Even for information interaction with users
 */



/// The important part is based on JSON implementation
#include "json.h"
/// The beginning character of Base64 encode JSON string
#define JSON_BASE64_PREFIX '^'
/// The beginning character of readable JSON string
#define JSON_STRING_PREFIX '{'
/// The operator options for patch
#define JSON_PATCH_OP      "."
#define JSON_PATCH_SYNC    '|'
#define JSON_PATCH_MIRROR  '='
/// talk_st equal json_t
typedef json_t talk_struct;

/// talk_t is a pointer of talk_st
typedef talk_struct* talk_t;
/// tnull retruning by interface function hint that no information is available
/// tnull retruning by service function hint everything is fine
#define tnull_code          1
#define tnull       (talk_t)NULL
#define tnull_string        "NULL"
/// talk_t also can point to boole_t
typedef talk_t   boole_t;
/// ttrue retruning by interface function hint operator succceed
#define ttrue_code          2
#define ttrue      (boole_t)ttrue_code
#define ttrue_string        "ttrue"
/// tfalse retruning by interface function hint operator failed
/// tfalse retruning by service function hint service failed( need restart the service )
#define tfalse_code         3
#define tfalse     (boole_t)tfalse_code
#define tfalse_string       "tfalse"
/// terror retruning by interface function hint operator error
/// terror retruning by service function hint service error( don't restart the service )
#define terror_code         4
#define terror      (talk_t)terror_code
#define terror_string       "terror"

/// tpanic retruning when calling failed
#define tpanic_code         5
#define tpanic      (talk_t)tpanic_code
#define tpanic_string       "tpanic"



/** @brief The mapping between talk_t and string JSON

{ // beginning of json object, talk_t pointing to it, top json we call it json object

    // axp, axp_t pointing to it, "name" is identify as a string, "dim" is value as a string
    "name":"dim"

    // beginning of axp its identify is "info", axp_t pointing to it, "info" is identify as a string, {} follow is value as a json
    "info":
    { // beginning of json
		// axp, axp_t pointing to it, "age" is identify as a string, "30" is value as a number
        "age":30,

		// axp, axp_t pointing to it, "level" is identify as a string, "low" is value as a string
        "level":"low"

    } // ending of json
	// ending of axp its idenfity is "info"

} // ending of json object

summarize:
	Three value of data content:
		string/nubmer( const char * )
		integer( int )
		boolean( boole )
	Tree structure pointers:
		json(It be point by talk_t), {}
		axp(It could also be point by talk_t), attribute identify must be x(string), value can be json or x(string/integer/boolean)
		x(It could also be point by talk_t), actual data, content can be string/integer/boolean
*/



/**
 * @brief determines whether the talk is a x or not
 * @param[in] v
 * @return true for x, false for other
 * 		@retval true for x
 *  	@retval false for not
 */
boole       x_check( talk_t v );
/**
 * @brief create x(dynamic allocation), which content is string/number/pointer
 * @return string or integer
 * @return x
 * 		@retval x for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t		string2x( const char *string );
talk_t		number2x( int i );
talk_t		pointer2x( void *pointer );
talk_t		text2x( const char *string );
/**
 * @brief get the string/number/pointer of x(dynamic allocation)
 * @param[in] x
 * @return string or integer
 * 		@retval string for succeed
 *  	@retval NULL for error, the errno code will be sets
 */
const char *x2string( talk_t x );
int 		x2number( talk_t x );
void 	   *x2pointer( talk_t x );
const char *x2text( talk_t x );



/**
 * @brief determines whether the talk is or not is axp
 * @param[in] v
 * @return true for axp, false for other
 * 		@retval true for axp
 *  	@retval false for not axp, the errno code will be sets
 */
boole       axp_check( talk_t v );
/**
 * @brief create a axp(dynamic allocation)
 * @param[in] attr, attribute idenfity, this string only copy to json
 * @param[in] string, value in string type, this string only copy to json
 * @param[in] v, v can be x/json, this parameter is valid when string is NULL, this parameter will be linkin to the axp which will be returned
 * @return axp
 * 		@retval axp for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      axp_create( const char *attr, const char *string, talk_t v );
/**
 * @brief set attribute name of axp
 * @param[in] axp
 * @param[in] attr, this string only copy to json
 * @return operation success or failure
 * 		@retval true for succeed
 *  	@retval false for failure, the errno code will be sets
 */
boole       axp_set_name( talk_t axp, const char *attr );
/**
 * @brief set axp value which is string/number/pointer/json type
 * @param[in] axp
 * @param[in] string, this string only copy to axp
 * @return operation success or failure
 * 		@retval true for succeed
 *  	@retval false for failure, the errno code will be sets
 */
boole       axp_set_string( talk_t axp, const char *string );
boole       axp_set_number( talk_t axp, int i );
boole       axp_set_pointer( talk_t axp, void *pointer );
boole       axp_set_json( talk_t axp, talk_t json );

/**
 * @brief get the attribute name of axp, this value is also not available when the axp is released
 * @param[in] axp
 * @return attribute name
 * 		@retval string for succeed
 *  	@retval NULL for error, the errno code will be sets
 */
const char *axp_name( talk_t axp );
/**
 * @brief get the value of axp, which value in string/number/pointer/json type, this string/json value is also not available when the axp is released
 * @param[in] axp
 * @return value in string/number/pointer/json type
 * 		@retval string for succeed
 *  	@retval NULL for error, the errno code will be sets
 */
const char *axp_string( talk_t axp );
int         axp_number( talk_t axp );
void	   *axp_pointer( talk_t axp );
talk_t		axp_json( talk_t axp );



/**
 * @brief determines whether the talk is or not is json
 * @param[in] json
 * @return true for json, false for other
 * 		@retval true for json
 *  	@retval false for not json, the errno code will be sets
 */
boole       json_check( talk_t json );
/**
 * @brief create a json(dynamic allocation)
 * @param[in] axp, create empty json when this is NULL, not NULL will be first axp at new json when be create
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      json_create( talk_t axp );
/**
 * @brief set axp value in json, which value is string/number/pointer/json type
 * @param[in] json
 * @param[in] attr, attribute name
 * @param[in] string/number/pointer/json, when string value this string only copy to json
 * @param[in] format, printf format
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed
 */
boole       json_set_string( talk_t json, const char *attr, const char *string );
boole       json_set_number( talk_t json, const char *attr, int i );
boole       json_set_pointer( talk_t json, const char *attr, void *pointer );
boole       json_set_json( talk_t json, const char *attr, talk_t v );

/**
 * @brief strip a axp from json and return it
 * @param[in] json
 * @return attribute name
 * @return axp
 * 		@retval axp for succeed, the axp is disconnect from json
 *  	@retval NULL for cannot find corresponding axp
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t      json_cut_axp( talk_t json, const char *attr );
/**
 * @brief delete axp from json
 * @param[in] json
 * @return attribute name
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for error, the errno code will be sets
 */
boole       json_delete_axp( talk_t json, const char *attr );
/**
 * @brief attach/add axp to json
 * @param[in] json
 * @param[in] axp
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for error, the errno code will be sets
 */
boole       json_attach_axp( talk_t json, talk_t axp );
/**
 * @brief detach/cut axp to json
 * @param[in] json
 * @param[in] axp
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for error, the errno code will be sets
 */
boole       json_detach_axp( talk_t json, talk_t axp );

/**
 * @brief get the value of axp in json, which value in string/number/pointer/json type, this string/json value is also not available when the json is released
 * @param[in] json
 * @param[in] attr, attribute name
 * @return string/number/pointer/json
 * 		@retval string/number/pointer/json
 *  	@retval NULL for value is NULL
 *  	@retval NULL for error, the errno code will be sets
 */
const char *json_string( talk_t json, const char *attr );
int         json_number( talk_t json, const char *attr );
void       *json_pointer( talk_t json, const char *attr );
talk_t		json_json( talk_t json, const char *attr );

/**
 * @brief get axp in json, this value is also not available when the json is released
 * @param[in] json
 * @param[in] attr attribute name
 * @return axp
 *		@retval axp for succeed
 *		@retval NULL for value is NULL
 *		@retval NULL for error, the errno code will be sets
 */
talk_t      json_axp( talk_t json, const char *attr );
/**
 * @brief get the next axp of the json, this value is also not available when the json is released
 * @param[in] json
 * @param[in] axp return the first axp at the json when this is NULL, return NULL when this is the last axp
 * @return axp
 * 		@retval axp for succeed
 *  	@retval NULL for current axp is the last axp
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t      json_next( talk_t json, talk_t axp );
/**
 * @brief get the previous axp of the json, this value is also not available when the json is released
 * @param[in] json
 * @param[in] axp return the last axp at the json when this is NULL, return NULL when this is the first axp
 * @return axp
 * 		@retval axp for succeed
 *  	@retval NULL for current axp is the first axp
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t      json_prev( talk_t json, talk_t axp );
/**
 * @brief empty json 
 * @param[in] json
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole       json_empty( talk_t json );
/**
 * @brief get how many axp in the json
 * @param[in] json
 * @return number
 * 		@retval 0 for empty json
 *  	@retval >0 number of axp
 *  	@retval <0 for error, the errno code will be sets
 */
int         json_size( talk_t json );
/**
 * @brief sync the json by other json
 * @param[in] src, a pointer of json is refer json
 * @param[in] dest, a pointer of json will be modify
 * @return operation succeed or failed
 * 		@retval 1 for sync succeed
 * 		@retval 0 for no modify
 *  	@retval negative for error, the errno code will be sets
 */
boole       json_sync( talk_t src, talk_t dest );
/**
 * @brief patch(sync/mirror) the talk by other talk
 * @param[in] src, a pointer of json is refer json
 * @param[in] dest, a pointer of json will be modify
 * @return operation succeed or failed
 *		@retval 1 for patch succeed
 *		@retval 0 for no modify
 *		@retval negative for error, the errno code will be sets
 */
int 		json_patch( talk_t src, talk_t dest );



/**
 * @brief get the value of axp, which value can be x/json, this value is also not available when the axp is released
 * @param[in] axp
 * @return x/json
 *		@retval x/json for succeed
 *		@retval NULL for value is NULL
 *		@retval NULL for error, the errno code will be sets
 */
talk_t      axp_value( talk_t axp );
/**
 * @brief set axp value which can be x/json
 * @param[in] axp
 * @param[in] json, this parameter will be linkin to the axp
 * @return operation success or failure
 *		@retval true for succeed
 *		@retval false for failure, the errno code will be sets
 */
boole       axp_set_value( talk_t axp, talk_t v );
/**
 * @brief strip and return axp value, which value can be x/json
 * @param[in] axp
 * @return x/json
 * 		@retval x/json for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      axp_cut_value( talk_t axp );
/**
 * @brief delete axp value
 * @param[in] axp
 * @return operation success or failure
 * 		@retval true for succeed
 *  	@retval false for failure, the errno code will be sets
 */
boole       axp_delete_value( talk_t axp );
/**
 * @brief get value of axp in json, which value can be x/json, this value is also not available when the json is released
 * @param[in] json
 * @param[in] attr attribute name
 * @return x/json
 *		@retval x/json for succeed
 *		@retval NULL for value is NULL
 *		@retval NULL for error, the errno code will be sets
 */
talk_t      json_value( talk_t json, const char *attr );
/**
 * @brief strip a value from axp in the json and return it, which value can be x/json
 * @param[in] json
 * @return attribute name
 * @return x/json
 * 		@retval x/json for succeed, the x/json is disconnect from axp in json
 *  	@retval NULL for cannot find corresponding x/json
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t      json_cut_value( talk_t json, const char *attr );
/**
 * @brief set axp value in json, which value can be x/json
 * @param[in] json
 * @param[in] attr, attribute name
 * @param[in] x/json value, this json well be linked in json
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole       json_set_value( talk_t json, const char *attr, talk_t v );



/**
 * @brief create talk from json format string(dynamic allocation)
 * @param[in] string, json syntax string
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      string2json( const char *string );
/**
 * @brief create string from talk(dynamic allocation)
 * @param[in] json, a pointer of json
 * @return string
 * 		@retval string for succeed
 *  	@retval NULL error, the errno code will be sets
 */
char       *json2string( talk_t json );
/**
 * @brief load a JSON structure from a file(dynamic allocation)
 * @param[in] path, filename
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL for failed, the errno code will be sets
 */
talk_t      file2json( const char *path );
/**
 * @brief store the JSON structure to a file
 * @param[in] json, a pointer of json
 * @param[in] path, filename
 * @return the number of characters written to the file
 * 		@retval positive for succeed
 *  	@retval negative for error, the errno code will be sets
 */
int         json2file( talk_t json, const char *path );
/**
 * @brief store the JSON structure to a file and make it readable
 * @param[in] json
 * @param[in] pathformat, path format
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for error, the errno code will be sets
 */
boole      json_save( talk_t json, const char *pathformat, ... );
/**
 * @brief load a JSON structure from a file(dynamic allocation)
 * @param[in] pathformat, path format
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL for failed, the errno code will be sets
 */
talk_t	   json_load( const char *pathformat, ... );



/**
 * @brief dup the talk(dynamic allocation)
 * @param[in] json
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      talk_dup( talk_t json );
/**
 * @brief compare two talk are the same
 * @param[in] json
 * @param[in] json2
 * @return true or false
 * 		@retval true for same
 * 		@retval false for not smae
 */
boole       talk_equal( talk_t json, talk_t json2 );
/**
 * @brief free a json
 * @param[in] json
 * @return none
 */
void        talk_free( talk_t json );
/**
 * @brief print json/x to std output stream and make it readable
 * @param[in] json/x
 * @return how many byte write
 * 		@retval >=0 for succeed
 *		@retval negative for error, the errno code will be sets
 */
int         talk_print( talk_t json );



#endif   /* ----- #ifndef H_LAND_TALK_H  ----- */

