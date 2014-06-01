#ifndef NODE_H
#define NODE_H
/* flags used, not all are in use,.. all code should reference the defines
  not the numbers
*/

#define F_hidden	1
#define F_readonly	2
#define F_temp		4
#define F_todo		8
#define F_done		16
#define F_32		32
#define F_64		64
#define F_128		128
#define F_256		256
#define F_512		512
#define F_1024		1024
#define F_2048		2048
#define F_visible	4096

/* Structure of a node,.. don't acces them directly use the other functions
   in this file to acces them.
*/
typedef struct Node {
	struct Node *up,*down,*left,*right;
	char *data;
	unsigned int flags;
	unsigned char priority;
} Node;


/*	macros to determine if there is a node immedieatly next to the
   specified in a driection, returns 0 if there isn't the node if
   there is.
   
	Returns: node,  0 if none   
*/
#define node_up(node)		(node==0?0:node->up)
#define node_down(node)		(node==0?0:node->down)
#define node_right(node)	(node==0?0:node->right)
#define node_left(node)		(node==0?0:node->left)

/* setting and getting of flags 
*/
#define node_setflags(node,tflags)	{if(node!=0)node->flags=tflags;}
#define node_getflags(node)			(node->flags)

/* returns the state of the specified flag
	Returns: 1 if flag is set 0 if not
*/
#define node_getflag(node,flag)	(node==0?0:(node->flags&flag?1:0))

/*	sets the specified flag if state is 1, turns of the flag if state is 0
*/
void node_setflag(Node *node,int flag, int state);

/* toggles the specified flag, 

	Returns: 1 if flag were set 0 if flag were turned of
*/
int node_toggleflag(Node *node,int flag);


/*sets and gets the data for a node, does neccesary allocating
  and freeing as well.
*/
void node_setdata(Node *node,char *data);

/*
	Returns: pointer to data
*/
char *node_getdata(Node *node);

/* setting and getting of node priority
*/
unsigned char node_getpriority(Node *node);
void node_setpriority(Node *node, unsigned char priority);

/* allocates a new node, seta all data to zero

	Returns: new node
*/
Node *node_new();

/* frees a node an it's related variables
*/
void node_free(Node *node);

#endif /* NODE_H */
