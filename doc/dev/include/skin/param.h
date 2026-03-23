#ifndef H_LAND_PARAM_H
#define H_LAND_PARAM_H
/**
 * @file param.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief implementation of parameter structure and related functions
 * @details used to pass strings or talk_t when called, component's function need accept a pointer of parameter structure, strings and talk can build into parameter as options
 */

/// Parameter option type
#define PARAM_OPTION_TEXT    0
#define PARAM_OPTION_POINTER 1

/// Maximum options in one parameter structure
#define PARAM_OPTIONS_MAX    10

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

	// description the option type
	boole type[PARAM_OPTIONS_MAX];

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

} param_struct;
typedef param_struct* param_t;



/**
 * @brief create a structure of parameter from string of options description(dynamic allocation)
 * @param[in] string description for options in format "opt1,opt2,opt3", can include JSON objects like '{"key":"value"}'
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note The string is copied internally, caller can free the input string after call
 * @note Example formats:
 * @code
 * param_t p1 = param_create("eth0,192.168.1.1,24");           // Simple comma-separated values
 * param_t p2 = param_create('iface,{"ip":"1.1.1.1"},active'); // Mixed string and JSON
 * param_t p3 = param_create(NULL);                            // Empty parameter
 * @endcode
 * @see param_free to release the parameter structure
 */
param_t     param_create( const char *string );

/**
 * @brief create a structure of parameter from json description(dynamic allocation)
 * @param[in] json a JSON object whose attributes will be converted to parameters (1st-4th attrs used)
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note The json is duplicated internally, caller retains ownership of input json
 * @note Looks for attributes named "1" through "10" (up to PARAM_OPTIONS_MAX) in the JSON
 * @see param_create for string-based parameter creation
 */
param_t     param_build( talk_t json );

/**
 * @brief create a structure of parameter from pointers(dynamic allocation)
 * @param[in] pointer first pointer option (can be NULL)
 * @param[in] pointer2 second pointer option (can be NULL)
 * @param[in] pointer3 third pointer option (can be NULL)
 * @param[in] pointer4 fourth pointer option (can be NULL)
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note Pointers are stored as references, caller must ensure they remain valid during parameter use
 * @warning The pointers are NOT copied or freed by param_free()
 * @see param_insertp, param_addp for adding more pointers
 */
param_t 	param_found( void *pointer, void *pointer2, void *pointer3, void *pointer4 );

/**
 * @brief free a parameter and all its associated resources
 * @param[in] parameter the parameter structure to free
 * @return none
 * @note This frees all internally allocated memory including string options and talk_t options
 * @note Pointer options (PARAM_OPTION_POINTER) are NOT freed - caller retains ownership
 * @warning Accessing the parameter or its returned values after free is undefined behavior
 * @see param_create, param_build, param_found
 */
void        param_free( param_t parameter );



/**
 * @brief create a structure of parameter and insert a string option at the head
 * @param[out] parameter existing parameter or NULL to create new
 * @param[in] option string option to insert at the head (will be copied)
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note If parameter is NULL, a new parameter structure is created
 * @note The option string is copied internally
 * @see param_adds for adding at the end
 * @see param_insertt for inserting a talk_t option
 * @see param_insertp for inserting a pointer option
 */
param_t 	param_inserts( param_t param, const char *option );

/**
 * @brief create a structure of parameter and insert a talk_t option at the head
 * @param[out] parameter existing parameter or NULL to create new
 * @param[in] option talk_t option to insert at the head (duplicated internally)
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note If parameter is NULL, a new parameter structure is created
 * @note The talk_t is duplicated internally, caller retains ownership of input
 * @see param_addt for adding at the end
 */
param_t 	param_insertt( param_t param, const talk_t option );

/**
 * @brief create a structure of parameter and insert a pointer option at the head
 * @param[out] parameter existing parameter or NULL to create new
 * @param[in] option pointer option to insert at the head
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note If parameter is NULL, a new parameter structure is created
 * @warning The pointer is stored as reference, not copied
 * @see param_addp for adding at the end
 */
param_t 	param_insertp( param_t param, void *option );

/**
 * @brief create a structure of parameter and add a string option at the end
 * @param[out] parameter existing parameter or NULL to create new
 * @param[in] option string option to add at the end (will be copied)
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note If parameter is NULL, a new parameter structure is created
 * @note The option string is copied internally
 * @see param_inserts for inserting at the head
 */
param_t 	param_adds( param_t param, const char *option );

/**
 * @brief create a structure of parameter and add a talk_t option at the end
 * @param[out] parameter existing parameter or NULL to create new
 * @param[in] option talk_t option to add at the end
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note If parameter is NULL, a new parameter structure is created
 * @note The talk_t is duplicated internally
 * @see param_insertt for inserting at the head
 */
param_t 	param_addt( param_t param, const talk_t option );

/**
 * @brief create a structure of parameter and add a pointer option at the end
 * @param[out] parameter existing parameter or NULL to create new
 * @param[in] option pointer option to add at the end
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note If parameter is NULL, a new parameter structure is created
 * @warning The pointer is stored as reference, not copied
 * @see param_insertp for inserting at the head
 */
param_t 	param_addp( param_t param, void *option );

/**
 * @brief adjust parameter start position by moving forward (skipping options from the beginning)
 * @param[in] parameter the parameter structure
 * @param[in] number number of positions to shift forward
 * @return operation succeed or failed
 *	@retval true for succeed
 *	@retval false for failed, errno will be set
 * @note After shift, param_string(param, 1) returns what was param_string(param, number+1)
 * @note Example: if parameter has [a,b,c,d], param_shift(p, 2) makes it [c,d]
 * @see param_unshift for moving backward
 * @see param_size to get current option count
 */
boole       param_shift( param_t parameter, int number );

/**
 * @brief adjust parameter start position by moving backward (restoring skipped options)
 * @param[in] parameter the parameter structure
 * @param[in] number number of positions to unshift backward
 * @return operation succeed or failed
 *	@retval true for succeed
 *	@retval false for failed, errno will be set
 * @note This reverses the effect of param_shift()
 * @note Cannot unshift more than previously shifted
 * @see param_shift
 */
boole       param_unshift( param_t parameter, int number );

/**
 * @brief import a talk_t and add it at the end of parameter
 * @param[out] parameter existing parameter or NULL to create new
 * @param[in] json talk_t to import at the end (ownership transferred, do NOT free after call)
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note If parameter is NULL, a new parameter structure is created
 * @warning The json is NOT duplicated, ownership is transferred to the parameter. Do not free or modify after call
 * @see param_import2 for importing two talk_t options
 */
param_t     param_import( param_t parameter, talk_t json );

/**
 * @brief import two talk_t and add them at the end of parameter
 * @param[out] parameter existing parameter or NULL to create new
 * @param[in] json first talk_t to import (ownership transferred)
 * @param[in] json2 second talk_t to import (ownership transferred)
 * @return parameter
 * 	@retval parameter for succeed
 *  	@retval NULL for error, errno will be set
 * @note If parameter is NULL, a new parameter structure is created
 * @warning Neither json nor json2 are duplicated, ownership is transferred. Do not free or modify after call
 * @see param_import for importing single talk_t
 */
param_t     param_import2( param_t parameter, talk_t json, talk_t json2 );



/**
 * @brief get how many options in parameter
 * @param[in] parameter the parameter structure
 * @return number of options
 * 	@retval positive or zero for succeed (current option count)
 *  	@retval negative for error, errno will be set
 * @note Returns the count of available options after any shift operations
 * @note Example: if parameter has [a,b,c] and shifted by 1, returns 2
 * @see param_shift, param_unshift
 */
int         param_size( param_t parameter );

/**
 * @brief get string option at specified position from parameter
 * @param[in] parameter the parameter structure
 * @param[in] serial serial number (starts from 1), use -1 to get the last option
 * @return string option
 * 	@retval string pointer to the option value, valid only until param_free() is called
 * 	@retval NULL if serial is out of range or parameter is NULL, errno will be set
 * @note The returned pointer points to internal memory, do NOT free it
 * @warning The pointer becomes invalid after param_free() is called
 * @note Special values for serial:
 * @code
 * param_t p = param_create("opt1,opt2,opt3");
 * const char *first = param_string(p, 1);   // returns "opt1"
 * const char *second = param_string(p, 2);  // returns "opt2"
 * const char *last = param_string(p, -1);   // returns "opt3" (last option)
 * const char *invalid = param_string(p, 10); // returns NULL (out of range)
 * param_free(p);  // After this, all returned pointers become invalid!
 * @endcode
 * @see param_talk for getting talk_t options
 * @see param_pointer for getting pointer options
 */
const char *param_string( param_t parameter, int serial );

/**
 * @brief get talk_t option at specified position from parameter
 * @param[in] parameter the parameter structure
 * @param[in] serial serial number (starts from 1), use -1 to get the last option
 * @return talk option
 * 	@retval talk pointer to the talk_t option, valid only until param_free() is called
 * 	@retval NULL if serial is out of range, parameter is NULL, or the option is not a talk_t, errno will be set
 * @note The returned talk_t points to internal memory, do NOT free it
 * @warning The pointer becomes invalid after param_free() is called
 * @note Only returns non-NULL if the option was inserted as talk_t using param_insertt() or param_addt()
 * @note String options created by param_create() are NOT talk_t options
 * @see param_insertt, param_addt for creating talk_t options
 * @see param_string for getting string options
 * @see param_pointer for getting pointer options
 */
talk_t      param_talk( param_t parameter, int serial );

/**
 * @brief get pointer option at specified position from parameter
 * @param[in] parameter the parameter structure
 * @param[in] serial serial number (starts from 1), use -1 to get the last option
 * @return pointer option
 * 	@retval pointer the pointer value stored in the option
 * 	@retval NULL if serial is out of range, parameter is NULL, or the option is not a pointer, errno will be set
 * @note Only returns non-NULL if the option was inserted as pointer using param_insertp() or param_addp()
 * @warning The returned pointer is the original pointer passed to param_insertp/addp, ownership remains with caller
 * @see param_insertp, param_addp for creating pointer options
 * @see param_string for getting string options
 * @see param_talk for getting talk_t options
 */
void       *param_pointer( param_t parameter, int serial );

/**
 * @brief get the combined string representation of all parameters
 * @param[in] parameter the parameter structure
 * @return string of the options description in format "opt1,opt2,opt3"
 * 	@retval string for succeed, valid only until param_free() or next modification
 *  	@retval NULL for error, errno will be set
 * @note The returned string points to internal buffer, do NOT free it
 * @warning The pointer becomes invalid after param_free() or any modification function is called
 * @note This reconstructs the parameter string from current options
 * @note Example: param_create("a,b") -> param_adds(p, "c") -> param_combine() returns "a,b,c"
 * @see param_create for the inverse operation
 */
const char *param_combine( param_t parameter );



#endif   /* ----- #ifndef H_LAND_PARAM_H  ----- */
