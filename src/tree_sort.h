#ifndef TREE_SORT_H
#define TREE_SORT_H
#include "node.h"

/* qucik sorts the siblings of the current node alphabetically,
   with undone todo items at top, then done todo items, and last
   non todo items
   
   Returns: topmost of the node after sorting
*/
Node *node_sort_siblings (Node *node);

/* checks/unchecks the todo boxes of parents according to the status of 
   current node and	siblings.
*/

#endif /* TREE_SORT_H */
