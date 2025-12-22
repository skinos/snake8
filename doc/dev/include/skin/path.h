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
#define OBJ_MAX_LEVEL 20

/// object structure: this structure can locate the components
/// example: "myproject@component" of component path description for object
///    		 "myproject" is project name
///    		 "component" is component name
struct com_st;
typedef struct obj_st
{
    // how many layer
    // example: level is 3
    int level;
    // all layer pointer
    // example: layer[0] point to "top_object"       obj_layer( obj, 1 );
    // 			layer[1] point to "second_object"    obj_layer( obj, 2 );
    // 			layer[2] point to "third_object"     obj_layer( obj, 3 );
    char *layer[OBJ_MAX_LEVEL];

	// project name
    char *prj;
	// component name
    char *obj;
    // actual pathname buffer
    char pathbuf[PATH_MAX];
	// component type, such as:
	// 		component file
	// 		execute file
	//		shell file
	// 		configure file
	//		kernel module file
    signed char    type;
    // corresponding pointer of component structure
    struct com_st *com;                                                // need free

    // corresponding pointer of register structure for write
    void          *wreg;
    // corresponding pointer of register structure for read
    void          *rreg;

    // buffer where the string actual store
    char *buffer;                                                      // need free
    // readable compiled string for this object
    char *string;                                                      // need free
} obj_struct;
typedef obj_struct* obj_t;

/**
 * @brief create a structure of object from string description of component(dynamic allocation)
 * @param[in] string, description for component
 * @return object
 * 		@retval obj for succeed
 *  	@retval NULL for error, errno will be set
 */
obj_t       obj_create( const char *string );
/**
 * @brief free a object
 * @param[in] object
 * @return none
 */
void        obj_free( obj_t object );
/**
 * @brief get project name in object, this value is also not available when the object is released
 * @param[in] object
 * @return project name
 *		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *obj_prj( obj_t object );
/**
 * @brief get component name from object, this value is also not available when the object is released
 * @param[in] object
 * @return component name
 *		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *obj_com( obj_t object );
/**
 * @brief get component type from object
 * @param[in] object
 * @return component type
 * 		@retval COM_FILE_KO for kernel module
 * 		@retval COM_FILE_LIB for component
 * 		@retval COM_FILE_SHELL for shell
 * 		@retval COM_FILE_EXECUTE for execute file
 *  	@retval negative error, errno will be set
 */
int         obj_type( obj_t object );
/**
 * @brief get how many layer in object
 * @param[in] object
 * @return number of layer
 *		@retval positive for succeed
 *		@retval negative for error
 */
int         obj_level( obj_t object );
/**
 * @brief gets the property name of the specified level from object, this value is also not available when the object is released
 * @param[in] object
 * @param[in] level layer number( It starts 1 )
 * @return property name
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *obj_layer( obj_t object, int level );
/**
 * @brief get string of component path description from object, this value is also not available when the object is released
 * @param[in] object
 * @return string description of component path
 * 		@retval string succeed
 *  	@retval NULL for error, errno will be set
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
    // example: layer[0] point to "top_attr"        attr_layer( obj, 1 );
    // 			layer[1] point to "second_attr"     attr_layer( obj, 2 );
    // 			layer[2] point to "third_attr"      attr_layer( obj, 3 ); 
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
 * @brief create a structure of attribute depend on string of attributes description
 * @param[in] attribute string description for attribute
 * @return attribute
 * 		@retval attribute for succeed
 *  	@retval NULL for error, errno will be set
 */
attr_t      attr_create( const char *attribute );
/**
 * @brief free a attribute
 * @param[in] attribute
 * @return none
 */
void        attr_free( attr_t attribute );
/**
 * @brief get how many layer in attribute
 * @param[in] attribute
 * @return number of layer
 *		@retval positive for succeed
 *		@retval negative for error
 */
int         attr_level( attr_t attribute );
/**
 * @brief gets the property name of the specified level from attribute, this value is also not available when the attribute is released
 * @param[in] attribute
 * @param[in] level layer number( It starts 1 )
 * @return property name
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *attr_layer( attr_t attribute, int level );
/**
 * @brief get string of attributes description from attribute, this value is also not available when the attribute is released
 * @param[in] attribute
 * @return string of attributes description
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *attr_combine( attr_t attribute );





/**
 * @brief get value from json depend on attribute, this value is also not available when the json is released
 * @param[in] json
 * @param[in] attribute
 * @return x/json
 *		@retval x/json for succeed
 *		@retval NULL for error, errno will be set
 */
talk_t      attr_get( talk_t json, attr_t attribute );
talk_t      attr_gets( talk_t json, const char *attribute, ... );
/**
 * @brief get value in string type from json depend on attribute
 * @param[out] buffer buffer the string will be store here
 * @param[in] buflen size of buffer
 * @param[in] json
 * @param[in] attribute
 * @return string
 *		@retval string for succeed
 *		@retval NULL for error, errno will be set
 */
const char *attr_get_string( char *buffer, int buflen, talk_t json, attr_t attribute );
const char *attr_gets_string( char *buffer, int buflen, talk_t json, const char *attribute, ... );
/**
 * @brief strip a value from json depend on attribute and return it
 * @param[in] json
 * @param[in] attribute
 * @return x/json
 *		@retval x/json for succeed
 *		@retval NULL for error, errno will be set
 */
talk_t      attr_cut( talk_t json, attr_t attribute );
talk_t      attr_cuts( talk_t json, const char *attribute, ... );
/**
 * @brief set a value into json depend on attribute
 * @param[in] json
 * @param[in] value x/json value
 * @param[in] attribute
 * @return return the operation is succeed or failed
 * 		@retval ttrue for succeed
 *  	@retval tfalse for failed, and errno code will be sets
 */
boole       attr_set( talk_t json, talk_t v, attr_t attribute );
boole       attr_sets( talk_t json, talk_t v, const char *attribute, ... );
/**
 * @brief set value in string type into json depend on attribute
 * @param[in] json
 * @param[in] value string
 * @param[in] attribute
 * @return return the operation is succeed or failed
 * 		@retval ttrue for succeed
 *  	@retval tfalse for failed, and errno code will be sets
 */
boole       attr_set_string( talk_t json, const char *v, attr_t attribute );
boole       attr_sets_string( talk_t json, const char *v, const char *attribute, ... );





#endif   /* ----- #ifndef H_LAND_PATH_H  ----- */

