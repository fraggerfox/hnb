#define F_hidden	1
#define F_readonly	2
#define F_temp		4
#define F_8			8
#define F_16		16
#define F_32		32
#define F_64		64
#define F_128		128
#define F_256		256
#define F_512		512
#define F_1024		1024
#define F_2048		2048
#define F_visible	4096


typedef struct Node {
	struct Node *up,*down,*left,*right;
	char *data;
	unsigned int flags;
} Node;

#define node_up(node)		node==0?0:node->up
#define node_down(node)		node==0?0:node->down
#define node_right(node)	node==0?0:node->right
#define node_left(node)		node==0?0:node->left

#define node_setflags(node,tflags)	if(node!=0)node->flags=tflags
#define node_getflags(node)			node==0?0:node->flags

Node *node_top(Node *node);
Node *node_bottom(Node *node);

void node_setdata(Node *node,char *data);
char *node_getdata(Node *node);

Node *node_alloc();
void node_free(Node *node);

