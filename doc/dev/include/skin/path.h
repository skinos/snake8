#ifndef H_LAND_PATH_H
#define H_LAND_PATH_H
/**
 * @file path.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief Implementation of structure and related functions for object path and attribute path
 * @details object path is used to locate components in system, attribute path is used to locate attributes in configure
 */



/// Maximum object level in one object structure
#define OBJ_MAX_LEVEL 2

/// object structure: this structure can locate the components
/// example: "myproject@component" of component path description for object
///    		 "myproject" is project name
///    		 "component" is component name
struct com_st;
typedef struct obj_st
{
    // how many layer
    // example: level is 2
    int level;
    // all layer pointer
    // example: layer[0] point to "top_object"       obj_layer( object, 1 );
    // 			layer[1] point to "second_object"    obj_layer( object, 2 );
    char *layer[OBJ_MAX_LEVEL];

	// project name
    char *prj;
	// component name
    char *obj;
    // corresponding pointer of component structure
    struct com_st *com;                                                // need free

    // buffer where the string actual store
    char *buffer;                                                      // need free
    // readable compiled string for this object
    char *string;                                                      // need free
} obj_struct;
typedef obj_struct* obj_t;

/**
 * @brief create a structure of object from string description of component(dynamic allocation)
 * @param[in] path component path, typically "project@component"
 * @return object
 * 	@retval obj for succeed
 *  	@retval NULL for error, errno will be set
 * @note One-segment input (e.g. registered alias "machine") is resolved with com_path(); on success level becomes 2 with both prj and obj set from the filesystem path
 * @note Two-part form "project@component" sets prj and component directly (OBJ_MAX_LEVEL is 2)
 * @note Example:
 * @code
 * obj_t obj1 = obj_create("land@machine");           // Basic component path
 * const char *prj = obj_prj(obj1);  // returns "land"
 * const char *com = obj_com(obj1);  // returns "machine"
 * obj_free(obj1);
 * @endcode
 * @see obj_free to release the object
 * @see obj_prj, obj_com to extract components
 */
obj_t       obj_create( const char *path );

/**
 * @brief free an object and all its associated resources
 * @param[in] object the object structure to free
 * @return none
 * @note This frees all internally allocated memory
 * @warning Accessing the object or its returned values after free is undefined behavior
 * @see obj_create
 */
void        obj_free( obj_t object );

/**
 * @brief get project name from object
 * @param[in] object the object structure
 * @return project name
 *	@retval string for succeed, valid only until obj_free() is called
 *  	@retval NULL for error, errno will be set
 * @note The returned pointer points to internal memory, do NOT free it
 * @note After successful obj_create(), obj_level() is 2 and both prj and obj are usually non-NULL; NULL prj is only for degenerate/manual structures (see obj_name())
 * @warning The pointer becomes invalid after obj_free() is called
 * @see obj_com for getting component name
 * @see obj_name for getting full path string
 */
const char *obj_prj( obj_t object );

/**
 * @brief get component name from object
 * @param[in] object the object structure
 * @return component name
 *	@retval string for succeed, valid only until obj_free() is called
 *  	@retval NULL for error, errno will be set
 * @note The returned pointer points to internal memory, do NOT free it
 * @warning The pointer becomes invalid after obj_free() is called
 * @see obj_prj for getting project name
 * @see obj_name for getting full path string
 */
const char *obj_com( obj_t object );

/**
 * @brief get number of layers in object path
 * @param[in] object the object structure
 * @return number of layers
 *	@retval positive for succeed (1 or 2 with OBJ_MAX_LEVEL)
 *	@retval 0 for error, errno will be set
 * @note After successful obj_create(), this is 2 (either from project@component or resolved single-component path)
 * @see obj_layer to get specific layer name
 */
int         obj_level( obj_t object );

/**
 * @brief get the property name of the specified layer from object
 * @param[in] object the object structure
 * @param[in] level layer number (starts from 1)
 * @return property name
 * 	@retval string for succeed, valid only until obj_free() is called
 *  	@retval NULL for error, errno will be set
 * @note Layer numbers start from 1; use -1 for the last segment
 * @note Example: for "land@machine", layer 1 is "land", layer 2 is "machine"
 * @warning level must be in range 1..obj_level(object), or -1; otherwise behavior is undefined
 * @warning The pointer becomes invalid after obj_free() is called
 * @see obj_level to get total layer count
 */
const char *obj_layer( obj_t object, int level );

/**
 * @brief get full component path string from object
 * @param[in] object the object structure
 * @return string description of component path
 * 	@retval string for succeed, valid only until obj_free() is called
 *  	@retval NULL for error, errno will be set
 * @note Returns the full path in format "project@component"
 * @warning The pointer becomes invalid after obj_free() is called
 * @see obj_prj, obj_com for getting individual components
 */
const char *obj_name( obj_t object );



/// Maximum attributes level in one attr structure
#define ATTR_MAX_LEVEL 20

/// attribute structure: this structure can locate the attributes in component
/// example: "top_attr/second_attr/third_attr" of string description for attribute
///    		 "top_attr" is level 1 layer of attribute
///    		 "second_attr" is level 2 layer of attribute
///    		 "third_attr" is level 3 layer of attribute
typedef struct attr_st
{
    // how many layer
    // example: level is 3
    int level;

    // all layer pointer
    // example: layer[0] point to "top_attr"        attr_layer( attribute, 1 );
    // 			layer[1] point to "second_attr"     attr_layer( attribute, 2 );
    // 			layer[2] point to "third_attr"      attr_layer( attribute, 3 ); 
    char *layer[ATTR_MAX_LEVEL];

    // buffer where the string actual store
    char *buffer;                                                      // need free

    // readable compiled string for this attr
    char *string;                                                      // need free

	// description the attr is modifyied
	boole dirty;

} attr_struct;
typedef attr_struct* attr_t;

/**
 * @brief create a structure of attribute from string description
 * @param[in] attribute attribute path in format "level1/level2/level3" (up to 20 levels)
 * @return attribute
 * 	@retval attribute for succeed
 *  	@retval NULL for error, errno will be set
 * @note Attribute paths use '/' as separator
 * @note Maximum ATTR_MAX_LEVEL (20) levels supported
 * @note Example:
 * @code
 * attr_t a1 = attr_create("ip");                    // Single level
 * attr_t a2 = attr_create("wan/ip");               // Two levels
 * attr_t a3 = attr_create("network/eth0/config/ip"); // Multiple levels
 * int levels = attr_level(a3);  // returns 4
 * const char *layer1 = attr_layer(a3, 1);  // returns "network"
 * attr_free(a2);
 * @endcode
 * @see attr_free to release the attribute
 * @see attr_get, attr_set for using attributes with JSON
 */
attr_t      attr_create( const char *attribute );

/**
 * @brief free an attribute and all its associated resources
 * @param[in] attribute the attribute structure to free
 * @return none
 * @note This frees all internally allocated memory
 * @warning Accessing the attribute or its returned values after free is undefined behavior
 * @see attr_create
 */
void        attr_free( attr_t attribute );

/**
 * @brief get number of levels in attribute path
 * @param[in] attribute the attribute structure
 * @return number of levels
 *	@retval positive for succeed (1-20 levels)
 *	@retval 0 for error, errno will be set
 * @note Returns the count of path components separated by '/'
 * @note Example: "wan/ip" has 2 levels, "network/eth0/config" has 3 levels
 * @see attr_layer to get specific level name
 */
int         attr_level( attr_t attribute );

/**
 * @brief get the property name of the specified level from attribute
 * @param[in] attribute the attribute structure
 * @param[in] level level number (starts from 1)
 * @return property name
 * 	@retval string for succeed, valid only until attr_free() is called
 *  	@retval NULL for error, errno will be set
 * @note Level numbers start from 1
 * @note Example: for "network/eth0/config/ip", level 1 is "network", level 2 is "eth0"
 * @warning The pointer becomes invalid after attr_free() is called
 * @see attr_level to get total level count
 */
const char *attr_layer( attr_t attribute, int level );

/**
 * @brief get full attribute path string from attribute structure
 * @param[in] attribute the attribute structure
 * @return string of attributes description
 * 	@retval string for succeed, valid only until attr_free() is called
 *  	@retval NULL for error, errno will be set
 * @note Returns the full path in format "level1/level2/level3"
 * @warning The pointer becomes invalid after attr_free() is called
 * @see attr_create for the inverse operation
 */
const char *attr_combine( attr_t attribute );



/**
 * @brief get value from JSON using attribute path (attr_t version)
 * @param[in] json the source JSON object
 * @param[in] attribute the attribute path as attr_t (e.g., attr_create("user/name"))
 * @return x/json the value at the specified path
 *	@retval x/json for succeed, pointer valid while json exists
 *	@retval NULL for path not found or error, errno will be set
 * @note The returned value is a reference into the original json, NOT a copy
 * @warning Do NOT free the returned value - it belongs to the parent json
 * @note For simple static paths, use attr_create(); for dynamic paths, use attr_gets()
 * @note Example:
 * @code
 * talk_t json = string2json('{"user":{"name":"john","age":30}}');
 * talk_t name = attr_get(json, attr_create("user/name"));  // returns "john"
 * talk_t age = attr_get(json, attr_create("user/age"));    // returns 30
 * talk_t missing = attr_get(json, attr_create("user/phone")); // returns NULL
 * // No need to free name/age - they reference into json
 * talk_free(json);
 * @endcode
 * @see attr_gets for printf-style dynamic paths
 * @see attr_get_string for string return with buffer
 * @see attr_set for setting values
 */
talk_t      attr_get( talk_t json, attr_t attribute );

/**
 * @brief get value from JSON using printf-style attribute path (variadic version)
 * @param[in] json the source JSON object
 * @param[in] attribute printf-style format string for attribute path
 * @param[in] ... arguments for format string
 * @return x/json the value at the specified path
 *	@retval x/json for succeed, pointer valid while json exists
 *	@retval NULL for path not found or error, errno will be set
 * @note This is the variadic version of attr_get() - useful for dynamic paths
 * @warning Do NOT free the returned value - it belongs to the parent json
 * @note Example:
 * @code
 * talk_t json = string2json('{"users":{"john":{"age":30},"jane":{"age":25}}}');
 * const char *username = "john";
 * talk_t age = attr_gets(json, "users/%s/age", username);  // returns 30
 * talk_t age2 = attr_gets(json, "users/%s/age", "jane");   // returns 25
 * talk_free(json);
 * @endcode
 * @see attr_get for pre-built attr_t paths
 * @see attr_gets_string for string return with buffer
 */
talk_t      attr_gets( talk_t json, const char *attribute, ... );

/**
 * @brief get string value from JSON using attribute path (buffer version)
 * @param[out] buffer buffer to store the string value
 * @param[in] buflen size of buffer
 * @param[in] json the source JSON object
 * @param[in] attribute the attribute path as attr_t
 * @return string
 *	@retval string (pointer to buffer) for succeed
 *	@retval NULL for error or value is not a string, errno will be set
 * @note This version copies the string value into the provided buffer
 * @note Safe for non-string values - returns NULL if value is not a string
 * @note Example:
 * @code
 * char buf[256];
 * talk_t json = string2json('{"user":"john"}');
 * const char *name = attr_get_string(buf, sizeof(buf), json, attr_create("user"));
 * // buf now contains "john"
 * @endcode
 * @see attr_get for getting talk_t values
 * @see attr_gets_string for printf-style dynamic paths
 */
const char *attr_get_string( char *buffer, int buflen, talk_t json, attr_t attribute );

/**
 * @brief get string value from JSON using printf-style attribute path (variadic buffer version)
 * @param[out] buffer buffer to store the string value
 * @param[in] buflen size of buffer
 * @param[in] json the source JSON object
 * @param[in] attribute printf-style format string for attribute path
 * @param[in] ... arguments for format string
 * @return string
 *	@retval string (pointer to buffer) for succeed
 *	@retval NULL for error or value is not a string, errno will be set
 * @note Variadic version of attr_get_string()
 * @see attr_get_string for pre-built attr_t paths
 * @see attr_gets for talk_t return version
 */
const char *attr_gets_string( char *buffer, int buflen, talk_t json, const char *attribute, ... );

/**
 * @brief cut (remove and return) a value from JSON using attribute path
 * @param[in] json the source JSON object
 * @param[in] attribute the attribute path as attr_t
 * @return x/json the removed value
 *	@retval x/json for succeed, caller must free with talk_free()
 *	@retval NULL for path not found or error, errno will be set
 * @note The returned value is detached from the parent json - caller owns it
 * @warning Caller MUST free the returned value with talk_free()
 * @note The value is removed from the original json
 * @note Example:
 * @code
 * talk_t json = string2json('{"a":1,"b":2}');
 * talk_t b = attr_cut(json, attr_create("b"));  // json is now {"a":1}
 * // Use b...
 * talk_free(b);
 * talk_free(json);
 * @endcode
 * @see attr_cuts for printf-style dynamic paths
 * @see attr_delete for deleting without returning
 */
talk_t      attr_cut( talk_t json, attr_t attribute );

/**
 * @brief cut (remove and return) a value from JSON using printf-style path
 * @param[in] json the source JSON object
 * @param[in] attribute printf-style format string for attribute path
 * @param[in] ... arguments for format string
 * @return x/json the removed value
 *	@retval x/json for succeed, caller must free with talk_free()
 *	@retval NULL for path not found or error, errno will be set
 * @note Variadic version of attr_cut()
 * @warning Caller MUST free the returned value with talk_free()
 * @see attr_cut for pre-built attr_t paths
 */
talk_t      attr_cuts( talk_t json, const char *attribute, ... );

/**
 * @brief set a value in JSON using attribute path (attr_t version)
 * @param[in] json the target JSON object
 * @param[in] v the value to set (x/json type)
 * @param[in] attribute the attribute path as attr_t
 * @return operation result
 * 	@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note Creates intermediate objects if they don't exist
 * @note If v is a talk_t, it is duplicated (json takes ownership of copy)
 * @note Example:
 * @code
 * talk_t json = json_create(NULL);
 * attr_set(json, string2x("john"), attr_create("user/name"));
 * attr_set(json, number2x(30), attr_create("user/age"));
 * // json is now {"user":{"name":"john","age":30}}
 * talk_free(json);
 * @endcode
 * @see attr_sets for printf-style dynamic paths
 * @see attr_set_string for string values
 */
boole       attr_set( talk_t json, talk_t v, attr_t attribute );

/**
 * @brief set a value in JSON using printf-style attribute path (variadic version)
 * @param[in] json the target JSON object
 * @param[in] v the value to set (x/json type)
 * @param[in] attribute printf-style format string for attribute path
 * @param[in] ... arguments for format string
 * @return operation result
 * 	@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note Variadic version of attr_set()
 * @see attr_set for pre-built attr_t paths
 */
boole       attr_sets( talk_t json, talk_t v, const char *attribute, ... );

/**
 * @brief set a string value in JSON using attribute path (convenience function)
 * @param[in] json the target JSON object
 * @param[in] v the string value to set
 * @param[in] attribute the attribute path as attr_t
 * @return operation result
 * 	@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note This is a convenience wrapper that converts string to x internally
 * @note The string v is copied internally
 * @note Example:
 * @code
 * talk_t json = json_create(NULL);
 * attr_set_string(json, "john", attr_create("user/name"));
 * // Equivalent to: attr_set(json, string2x("john"), attr_create("user/name"));
 * talk_free(json);
 * @endcode
 * @see attr_set for general value setting
 * @see attr_sets_string for printf-style dynamic paths
 */
boole       attr_set_string( talk_t json, const char *v, attr_t attribute );

/**
 * @brief set a string value in JSON using printf-style attribute path (variadic convenience)
 * @param[in] json the target JSON object
 * @param[in] v the string value to set
 * @param[in] attribute printf-style format string for attribute path
 * @param[in] ... arguments for format string
 * @return operation result
 * 	@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note Variadic version of attr_set_string()
 * @see attr_set_string for pre-built attr_t paths
 */
boole       attr_sets_string( talk_t json, const char *v, const char *attribute, ... );



#endif   /* ----- #ifndef H_LAND_PATH_H  ----- */
