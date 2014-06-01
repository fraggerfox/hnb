
Node *tree_new();		/* creates a new tree and returns a node pointing to it 
							don't store this node for reference, keep updating
							your pointer into the tree with the pointers returned
							from other functions.
						*/

Node *node_root(Node *node);
						/* Given a node in a tree, this function returns a pointer
						   to the root of the tree
						*/

void tree_free();		/* frees a whole tree from memory, by reference to one of it's
							nodes
						*/

Node *node_top(Node *node);/*returns the topmost of the siblings on the level of node*/
Node *node_bottom(Node *node);/*returns the bottommost of the sibling on the lvel of node*/


Node *node_insert_up(Node *node); /* inserts a node above node, returns the new node*/
Node *node_insert_down(Node *node); 

Node * node_insert_right (Node * node); /*inserts a child for node, if there already
									is a child, 0 is returned*/

unsigned int nodes_up(Node *node);  /* the number of nodes above */
unsigned int nodes_down(Node *node); /* the number of nodes below */
unsigned int nodes_left (Node * node); /* the number of nodes to the left (level of node */
unsigned int nodes_right (Node * node);

#define node_no(node)		nodes_up(node)

Node *node_remove(Node *node); /* removes node and it's children, returns:
								the 'nearest' still existing node
								(up or down or left of specified node)*/

Node *node_match(char *match,Node *where); /*
										finds a node starting with match amongst the siblings 
										of where
										  */

Node *node_exact_match(char *match,Node *where);/* same as above, but exact match */

void node_swap(Node *nodeA,Node *nodeB); /* swaps the contents of two nodes */

Node *node_sort_siblings(Node *node); /* sorts the siblings of the current node,
										returns the new position of the specified node
										*/

void node_update_parents_todo(Node *pos);/* checks/unchecks the todo boxes of parents
											according to the status of current node and
											siblings.
										*/

int node_calc_complete(Node *node); /* returns an integer between 0 and 1000 according
									to the completion status of the nodes children*/

Node * node_recurse (Node * node); /* returns the next node, doing a recursive traversal 
									of the tree
									*/

Node * node_recursive_match (char *match, Node * where);
								/* returns the next node with a case insensitive 
								substring match from where.
								*/
