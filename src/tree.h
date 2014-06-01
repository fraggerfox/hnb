#ifndef TREE_H
#define TREE_H
#include "node.h"

/* creates a new tree and returns a node pointing to it don't store this node
   for reference, keep updating	your pointer into the tree with the pointers 
   returned	from other functions.
   
   Returns: new tree
*/
Node *tree_new();						


/* Given a node in a tree, this function returns a pointer to the root
   of the tree
   
   Returns: tree root   
*/
Node *node_root(Node *node);						

/* frees a whole tree from memory, by reference to one of it's nodes
*/
void tree_free(Node *node);						

/*returns the topmost of the siblings on the level of node

   Returns: level top
*/
Node *node_top(Node *node);

/*returns the bottommost of the sibling on the lvel of node

   Returns: level bottom
*/
Node *node_bottom(Node *node);

/* inserts a new node above node, returns the new node

   Returns: new node
*/
Node *node_insert_up(Node *node); 

/* inserts a new node below node, returns the new node

   Returns: new node
*/
Node *node_insert_down(Node *node); 

/*inserts a child for node, if there already is a child, 0 is returned

   Returns: new node
*/
Node * node_insert_right (Node * node); 

/* the number of nodes above 

	Returns: number of nodes
*/
unsigned int nodes_up(Node *node);

/* the number of nodes below 

	Returns: number of nodes
*/
unsigned int nodes_down(Node *node);

/* the number of nodes to the left (level of node)

	Returns: number of nodes
*/
unsigned int nodes_left (Node * node); 
unsigned int nodes_right (Node * node);

/* removes node and it's children, returns: the 'nearest' still existing node
  (up or down or left of specified node)

	Returns: nearby node
*/
Node *node_remove(Node *node); 

/* finds a node starting with match amongst the siblings of where
   returns 0 if no match found or no match string given

	Returns: matching node or 0
*/
Node *node_match(char *match,Node *where); 

/* same as above, but must match whole node

	Returns: matching node or 0
*/
Node *node_exact_match(char *match,Node *where);

/* returns the next node with a case insensitive substring match from where.

	Returns: matching node or 0
*/
Node * node_recursive_match (char *match, Node * where);

/* swaps the contents of two nodes
*/
void node_swap(Node *nodeA,Node *nodeB); 

/* qucik sorts the siblings of the current node alphabetically,
   with undone todo items at top, then done todo items, and last
   non todo items
   
   Returns: topmost of the node after sorting
*/
Node *node_sort_siblings(Node *node); 

/* checks/unchecks the todo boxes of parents according to the status of 
   current node and	siblings.
*/
void node_update_parents_todo(Node *pos);

/* Returns: an integer between 0 and 1000 according to the 
		completion status of the nodes children
*/
int node_calc_complete(Node *node); 

/* returns the next node, doing a recursive traversal of the tree 

	Returns: next recursive node or 0 if none
*/
Node * node_recurse (Node * node);									

/* returns the previous node, doing a recursive traversal of the tree backwards

	Returns: next back recursive node or 0 if none
*/
Node * node_backrecurse (Node * node);									

/* makes the specified node and it's lower siblings children
  of the node above, returns new pos of node

	Returns: node
*/
Node *node_raise(Node *node); 
								
/* opposite of above 

	Returns: node
*/
Node *node_lower(Node *node); 

/* returns the number of this node in the database 
*/
int node_no(Node *node); 
#endif /* TREE_H */
