Node *node_recurse(Node *node);
Node *node_top(Node *node);
Node *node_bottom(Node *node);

Node *node_insert_up(Node *node); 
Node *node_insert_down(Node *node); 
Node *node_insert_right(Node *node);

unsigned int nodes_right(Node *node);
unsigned int nodes_left(Node *node);
unsigned int nodes_up(Node *node);
unsigned int nodes_down(Node *node);

#define node_no(node)		nodes_up(node)
#define node_level(node)	nodes_left(node)

Node *node_remove(Node *node);
Node *node_match(char *match,Node *where);
Node *node_recursive_match(char *match,Node *where);
Node *node_exact_match(char *match,Node *where);

Node *tree_init();
Node *tree_root();
int tree_free();
void node_swap(Node *nodeA,Node *nodeB);
void node_update_parents_todo(Node *pos);
Node *node_sort_siblings(Node *node);

int node_calc_complete(Node *node);
