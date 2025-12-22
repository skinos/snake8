#ifndef H_LAND_LINK_H
#define H_LAND_LINK_H
/**
 * @file link.h
 * @author dimmalex@gmail.com
 * @version 8.0
 * @date 20240917
 * @brief implementation of general linker list
 */



/// linker node structure are embedded in other objects
typedef struct link_st
{
	// point to next linker node structure of next objects
    struct link_st *next;
	// point to previous linker node structure of previous objects
    struct link_st *prev;
} link_struct;
typedef link_struct* link_t;
/// linker queue pointer
typedef link_struct** lhead_t;



/**
 * @brief get the actual data structure by pointer of linker node
 * @param[in] node, pointer of linker node
 * @param[in] type, type of actual data structure
 * @param[in] member, name of linker node in actual data structure
 * @return a pointer of actual data
 */
#define link_entry( node, type, member ) ( ( type* ) ( ( char* ) ( node ) - ( unsigned long ) ( & ( ( type* ) 0 )->member) ) )

/**
 * @brief foreach a linker
 * @param[in] node, pointer of linker head
 * @param[in] head, linker queue
 * @return none, this is a macro of for();
 */
#define link_each( var, head )   for( ( var ) = ( *head ); NULL != ( var ); ( var = ( var )->next ) )

/**
 * @brief linker node initialize
 * @param[in] node, pointer of linker
 * @return success or failure
 * 		@retval true succeed
 *  	@retval false error
 */
boole   link_init( link_t node );
/**
 * @brief get first node of linker queue, is be used to determine linker queue is empty
 * @param[in] head, pointer of linker queue
 * @return pointer of first linker node
 * 		@retval pointer of node succeed
 *  	@retval NULL for empty
 *  	@retval NULL for error, errno will be set
 */
link_t  link_child( lhead_t head );

/**
 * @brief get number of child in the link
 * @param[in] head, pointer of link head structure
 * @return number of child in the link
 * 		@retval positive or zero succeed
 *  	@retval negative error
 */
int     link_size( lhead_t head );
/**
 * @brief delete a node from linker queue
 * @param[in] head, pointer of linker queue
 * @param[in] elm, pointer of linker node
 * @return success or failure
 *		@retval true for succeed
 *		@retval false for error, errno will be set
 */
boole   link_delete( lhead_t head, link_t elm );
/**
 * @brief delete a node from linker queue
 * @param[in] head, pointer of linker queue
 * @param[in] elm, pointer of linker node
 * @return the next
 */
link_t	link_remove( lhead_t head, link_t elm );



/**
 * @brief add a node to ending of linker queue
 * @param[in] head, pointer of linker queue
 * @param[in] elm, pointer of linker node
 * @return success or failure
 * 		@retval true for succeed
 *		@retval false for error, errno will be set
 */
boole   link_push( lhead_t head, link_t elm );
/**
 * @brief get a node from ending of linker queue
 * @param[in] head, pointer of linker queue
 * @return pointer of linker node
 * 		@retval node succeed
 *  	@retval NULL for empty
 *  	@retval NULL for error, errno will be set
 */
link_t  link_pop( lhead_t head );



/**
 * @brief insert a node to starting of linker queue
 * @param[in] head, pointer of linker queue
 * @param[in] elm, pointer of linker node
 * @param next pointer of linker node, insert to linker queue before this node, insert to linker queue starting when it is NULL
 * @return success or failure
 *		@retval true for succeed
 *		@retval false for error, errno will be set
 */
boole   link_insert( lhead_t head, link_t elm, link_t next );
/**
 * @brief take out a node from starting of linker queue
 * @param[in] head, pointer of linker queue
 * @return pointer of linker node
 * 		@retval node succeed
 *  	@retval NULL for empty
 *  	@retval NULL for error, errno will be set
 */
link_t  link_pull( lhead_t head );



#endif   /* ----- #ifndef H_LAND_LINK_H  ----- */

