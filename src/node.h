#ifndef NODE_H
#define NODE_H

/* Structure of a node,.. don't acces them directly use the other functions
   in this file to acces them.
*/
typedef struct Node {
	struct Node *up,*down,*left,*right;
	char *data;
	unsigned int flags;
	unsigned char priority;
} Node;

/* convenience macro for the rest of this header*/
#define if_node(a,b)		(a?b:0)

/*	macros to determine if there is a node immedieatly next to the
   specified in a driection, returns 0 if there isn't the node if
   there is.
   
	Returns: node,  0 if none   
*/
#define node_up(node)		if_node(node,node->up)
#define node_down(node)		if_node(node,node->down)
#define node_right(node)	if_node(node,node->right)
#define node_left(node)		if_node(node,node->left)

/* sets all the flags of a node, if it exists
	Returns: New flags, or 0 if node didn't exist
*/
#define node_setflags(node,tflags)	if_node(node,node->flags=tflags)


/* gets all the flags of a node, if it exists
	Returns: flags, or 0 if node didn't exist
*/
#define node_getflags(node)			if_node(node,(node->flags))


/* returns the state of the specified flag
	Returns: 1 if flag is set 0 if not
*/
#define node_getflag(node,flag)	if_node(node,(node->flags&flag?1:0))

/*	sets the specified flag if state is 1, turns of the flag if state is 0

*/
#define node_setflag(node,flag,state)\
	{if(state){node_setflags(node,node->flags|flag);}\
	else	{node_setflags(node, node->flags & ( flag  ^0xffff));}}

/* ansi c complained too much about value computed not used 
   so I reverted to a new macro function above not returinging flags
*/
#define old_node_setflag(node,flag,state)\
	(state?   node_setflags(node,node->flags|flag)\
	/*else*/: node_setflags(node, node->flags & ( flag  ^0xffff)) )


/* toggles the specified flag, 

	Returns: 1 if flag were set 0 if flag were turned of
*/
#define node_toggleflag(node,flag) (   node_setflags(node, node->flags^flag )   &flag)


/*sets and gets the data for a node, does neccesary allocating
  and freeing as well.
*/
char *node_setdata(Node *node,char *data);

/*
	Returns: pointer to data
*/
char *node_getdata(Node *node);

/* getting of node priority
	Returns: priority, or 0 if node didn't exist
*/
#define node_getpriority(node)		(node?(node->priority):0)

/* sets priority of a node, if it exists
	Returns: New priority, or 0 if node didn't exist
*/
#define node_setpriority(node,new_priority)	(node?(node->priority=new_priority):0)

/* allocates a new node, seta all data to zero

	Returns: new node
*/
Node *node_new();

/* frees a node an it's related variables
*/
void node_free(Node *node);

#endif /* NODE_H */
