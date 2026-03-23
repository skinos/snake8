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
/// tnull returning by interface function hint that no information is available
/// tnull returning by service function hint everything is fine
#define tnull_code          1
#define tnull       (talk_t)NULL
#define tnull_string        "NULL"
/// talk_t also can point to boole_t
typedef talk_t   boole_t;
/// ttrue returning by interface function hint operator succeed
#define ttrue_code          2
#define ttrue      (boole_t)ttrue_code
#define ttrue_string        "ttrue"
/// tfalse returning by interface function hint operator failed
/// tfalse returning by service function hint service failed( need restart the service )
#define tfalse_code         3
#define tfalse     (boole_t)tfalse_code
#define tfalse_string       "tfalse"
/// terror returning by interface function hint operator error
/// terror returning by service function hint service error( don't restart the service )
#define terror_code         4
#define terror      (talk_t)terror_code
#define terror_string       "terror"

/// tpanic returning when calling failed
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
		string/number( const char * )
		integer( int )
		boolean( boole )
	Tree structure pointers:
		json(It be point by talk_t), {}
		axp(It could also be point by talk_t), attribute identify must be x(string), value can be json or x(string/integer/boolean)
		x(It could also be point by talk_t), actual data, content can be string/integer/boolean
*/



/**
 * @brief determines whether the talk is a x or not
 * @param[in] v talk_t value to check
 * @return true for x, false for other
 * 		@retval true for x
 *  	@retval false for not
 */
boole       x_check( talk_t v );
/**
 * @brief create an x value (dynamic allocation) from string/number/pointer
 * @param[in] string string value (for string2x), or int i (for number2x), or void* pointer (for pointer2x)
 * @return talk_t x value (must be freed with talk_free)
 * 		@retval talk_t x for succeed
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t		string2x( const char *string );
talk_t		number2x( int i );
talk_t		pointer2x( void *pointer );
/**
 * @brief create x from text string (dynamic allocation), stores content as raw text without JSON escaping
 * @param[in] string text string value
 * @return talk_t x value (must be freed with talk_free)
 * 	@retval talk_t x for succeed
 *  	@retval NULL for error, the errno code will be sets
 * @note Unlike string2x which stores as JSON string, text2x preserves raw text content
 * @see string2x for JSON string storage
 */
talk_t		text2x( const char *string );
/**
 * @brief get the string/number/pointer value from x
 * @param[in] x talk_t x value to extract from
 * @return extracted value (string pointer, integer, or void pointer)
 * 		@retval string/number/pointer for succeed
 *  	@retval NULL/0 for error, the errno code will be sets
 * @note The returned string pointer is owned by the x, do not free it separately
 * @note The returned string becomes invalid after talk_free(x)
 */
const char *x2string( talk_t x );
int 		x2number( talk_t x );
void 	   *x2pointer( talk_t x );
const char *x2text( talk_t x );



/**
 * @brief determines whether the talk is or not is axp
 * @param[in] v talk_t value to check
 * @return true for axp, false for other
 * 		@retval true for axp
 *  	@retval false for not axp, the errno code will be sets
 */
boole       axp_check( talk_t v );
/**
 * @brief create an attribute-value pair (axp) (dynamic allocation)
 * @param[in] attr attribute name/identifier (copied internally)
 * @param[in] string string value (copied internally), use NULL if setting via v parameter
 * @param[in] v talk_t value when string is NULL, linked directly to the returned axp
 * @return axp
	@retval talk_t for succeed - caller must free with talk_free()
	@retval NULL for error, errno will be set
 * @note Parameter combination logic:
 *   - If string != NULL: creates axp with string value, v is ignored
 *   - If string == NULL && v != NULL: creates axp with v as value
 *   - If both NULL: creates axp with null value
 * @note The attr string is always copied internally
 * @note When using v, the v talk_t is linked (not copied) - ownership shared
 * @note Example:
 * @code
 * // Create axp with string value
 * talk_t axp1 = axp_create("name", "john", NULL);
 *
 * // Create axp with talk_t value
 * talk_t value = number2x(30);
 * talk_t axp2 = axp_create("age", NULL, value);
 * // Note: value is linked, don't free separately
 *
 * talk_free(axp1);
 * talk_free(axp2);  // This also frees the linked value
 * @endcode
 * @see axp_set_string, axp_set_json for modifying values
 */
talk_t      axp_create( const char *attr, const char *string, talk_t v );
/**
 * @brief set attribute name of axp
 * @param[in] axp axp talk_t pointer
 * @param[in] attr attribute name (copied to json)
 * @return operation success or failure
 * 		@retval true for succeed
 *  	@retval false for failure, the errno code will be sets
 */
boole       axp_set_name( talk_t axp, const char *attr );
/**
 * @brief set axp value which is string/number/pointer/json type
 * @param[in] axp axp talk_t pointer
 * @param[in] string string value (copied to axp)
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
 * @param[in] axp axp talk_t pointer
 * @return attribute name
 * 		@retval string for succeed
 *  	@retval NULL for error, the errno code will be sets
 */
const char *axp_name( talk_t axp );
/**
 * @brief get the value of axp, which value in string/number/pointer/json type, this string/json value is also not available when the axp is released
 * @param[in] axp axp talk_t pointer
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
 * @param[in] json json talk_t pointer
 * @return true for json, false for other
 * 		@retval true for json
 *  	@retval false for not json, the errno code will be sets
 */
boole       json_check( talk_t json );
/**
 * @brief create a json(dynamic allocation)
 * @param[in] axp first axp in new json (NULL for empty json)
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      json_create( talk_t axp );
/**
 * @brief set axp value in json, which value is string/number/pointer/json type
 * @param[in] json target json object
 * @param[in] attr attribute name (creates axp if not exists, updates if exists)
 * @param[in] string/i/pointer/v value to set (string is copied internally, json v is linked)
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
 * @param[in] json json object
 * @param[in] attr attribute name to cut
 * @return axp
 * 		@retval axp for succeed, the axp is disconnect from json
 *  	@retval NULL for cannot find corresponding axp
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t      json_cut_axp( talk_t json, const char *attr );
/**
 * @brief delete axp from json
 * @param[in] json json object
 * @param[in] attr attribute name to delete
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for error, the errno code will be sets
 */
boole       json_delete_axp( talk_t json, const char *attr );
/**
 * @brief attach/add axp to json
 * @param[in] json json talk_t pointer
 * @param[in] axp axp talk_t pointer
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for error, the errno code will be sets
 */
boole       json_attach_axp( talk_t json, talk_t axp );
/**
 * @brief detach/cut axp from json (disconnect axp without freeing it)
 * @param[in] json json talk_t pointer
 * @param[in] axp axp talk_t pointer
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for error, the errno code will be sets
 */
boole       json_detach_axp( talk_t json, talk_t axp );

/**
 * @brief get the value of axp in json, which value in string/number/pointer/json type, this string/json value is also not available when the json is released
 * @param[in] json json talk_t pointer
 * @param[in] attr attribute name
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
 * @param[in] json json talk_t pointer
 * @param[in] attr attribute name
 * @return axp
 *		@retval axp for succeed
 *		@retval NULL for value is NULL
 *		@retval NULL for error, the errno code will be sets
 */
talk_t      json_axp( talk_t json, const char *attr );
/**
 * @brief get the next axp of the json, this value is also not available when the json is released
 * @param[in] json json talk_t pointer
 * @param[in] axp return the first axp at the json when this is NULL, return NULL when this is the last axp
 * @return axp
 * 		@retval axp for succeed
 *  	@retval NULL for current axp is the last axp
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t      json_next( talk_t json, talk_t axp );
/**
 * @brief get the previous axp of the json, this value is also not available when the json is released
 * @param[in] json json talk_t pointer
 * @param[in] axp return the last axp at the json when this is NULL, return NULL when this is the first axp
 * @return axp
 * 		@retval axp for succeed
 *  	@retval NULL for current axp is the first axp
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t      json_prev( talk_t json, talk_t axp );
/**
 * @brief empty json 
 * @param[in] json json talk_t pointer
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole       json_empty( talk_t json );
/**
 * @brief get how many axp in the json
 * @param[in] json json talk_t pointer
 * @return number
 * 		@retval 0 for empty json
 *  	@retval >0 number of axp
 *  	@retval -1 for error, the errno code will be sets
 */
int         json_size( talk_t json );
/**
 * @brief sync the json by other json
 * @param[in] src source json to reference
 * @param[in] dest destination json to modify
 * @return operation succeed or failed
 * 		@retval 1 for sync succeed
 * 		@retval 0 for no modify
 * @note Invalid src/dst (fails json_check) aborts via assert(); errno is not used for that path.
 */
int         json_sync( talk_t src, talk_t dest );
/**
 * @brief patch(sync/mirror) the talk by other talk
 * @param[in] src source json to reference
 * @param[in] dest destination json to modify
 * @return operation succeed or failed
 *		@retval 1 for patch succeed
 *		@retval 0 for no modify
 * @note Invalid src/dst (fails json_check) aborts via assert(); errno is not used for that path.
 */
int 		json_patch( talk_t src, talk_t dest );



/**
 * @brief get the value of axp, which value can be x/json, this value is also not available when the axp is released
 * @param[in] axp axp talk_t pointer
 * @return x/json
 *		@retval x/json for succeed
 *		@retval NULL for value is NULL
 *		@retval NULL for error, the errno code will be sets
 */
talk_t      axp_value( talk_t axp );
/**
 * @brief set axp value which can be x/json
 * @param[in] axp axp to set value on
 * @param[in] v talk_t value (x or json) to link to axp
 * @return operation success or failure
 *		@retval true for succeed
 *		@retval false for failure, the errno code will be sets
 */
boole       axp_set_value( talk_t axp, talk_t v );
/**
 * @brief strip and return axp value, which value can be x/json
 * @param[in] axp axp talk_t pointer
 * @return x/json
 * 		@retval x/json for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      axp_cut_value( talk_t axp );
/**
 * @brief delete axp value
 * @param[in] axp axp talk_t pointer
 * @return operation success or failure
 * 		@retval true for succeed
 *  	@retval false for failure, the errno code will be sets
 */
boole       axp_delete_value( talk_t axp );
/**
 * @brief get value of axp in json, which value can be x/json, this value is also not available when the json is released
 * @param[in] json json talk_t pointer
 * @param[in] attr attribute name
 * @return x/json
 *		@retval x/json for succeed
 *		@retval NULL for value is NULL
 *		@retval NULL for error, the errno code will be sets
 */
talk_t      json_value( talk_t json, const char *attr );
/**
 * @brief strip a value from axp in the json and return it, which value can be x/json
 * @param[in] json json object
 * @param[in] attr attribute name whose value to cut
 * @return x/json
 * 		@retval x/json for succeed, the x/json is disconnect from axp in json
 *  	@retval NULL for cannot find corresponding x/json
 *  	@retval NULL for error, the errno code will be sets
 */
talk_t      json_cut_value( talk_t json, const char *attr );
/**
 * @brief set axp value in json, which value can be x/json
 * @param[in] json json talk_t pointer
 * @param[in] attr attribute name
 * @param[in] x/json value, this json well be linked in json
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, the errno code will be sets
 */
boole       json_set_value( talk_t json, const char *attr, talk_t v );



/**
 * @brief create talk from json format string(dynamic allocation)
 * @param[in] string JSON syntax string
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      string2json( const char *string );
/**
 * @brief create string from talk(dynamic allocation)
 * @param[in] json json pointer
 * @return string
 * 		@retval string for succeed
 *  	@retval NULL error, the errno code will be sets
 */
char       *json2string( talk_t json );
/**
 * @brief load a JSON structure from a file(dynamic allocation)
 * @param[in] path file path
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL for failed, the errno code will be sets
 */
talk_t      file2json( const char *path );
/**
 * @brief store the JSON structure to a file
 * @param[in] json json pointer
 * @param[in] path file path
 * @return the number of characters written to the file
 * 		@retval positive for succeed
 *  	@retval negative for error, the errno code will be sets
 */
int         json2file( talk_t json, const char *path );
/**
 * @brief store the JSON structure to a file and make it readable
 * @param[in] json json talk_t pointer
 * @param[in] pathformat file path format string
 * @return operation succeed or failed
 * 		@retval true for succeed
 *  	@retval false for error, the errno code will be sets
 */
boole      json_save( talk_t json, const char *pathformat, ... );
/**
 * @brief load a JSON structure from a file(dynamic allocation)
 * @param[in] pathformat file path format string
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL for failed, the errno code will be sets
 */
talk_t	   json_load( const char *pathformat, ... );



/**
 * @brief dup the talk(dynamic allocation)
 * @param[in] json json talk_t pointer
 * @return json
 * 		@retval json for succeed
 *  	@retval NULL error, the errno code will be sets
 */
talk_t      talk_dup( talk_t json );
/**
 * @brief compare two talk are the same
 * @param[in] json json talk_t pointer
 * @param[in] json2 second json talk_t pointer to compare
 * @return true or false
 * 		@retval true for same
 * 		@retval false for not same
 */
boole       talk_equal( talk_t json, talk_t json2 );
/**
 * @brief free a json
 * @param[in] json json talk_t pointer
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

