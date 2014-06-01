/*
there is one a little bit strange thing about
this tree, it have the root at the top but
at the left.. (like the model presented to the 
user) */

#include <stdio.h>

#include <assert.h>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"
#include "tree.h"

static Node *root = 0;

Node *
node_recurse (Node * node)
{

  if (node_right (node))
    return (node_top(node_right (node)));
  if (node_down (node))
    return (node_down (node));

  while (node_left (node))
    {
      if (node_down (node_left (node)))
	return (node_down (node_left (node)));
      node = node_left (node);
    };
  return (0);
}

Node *
node_top (Node * node)
{
  Node *tnode;
  if (node == 0)
    return (0);
  tnode = node;

  while (node_up (tnode) != 0)
    tnode = node_up (tnode);
  return (tnode);
}

Node *
node_bottom (Node * node)
{
  Node *tnode;
  if (node == 0)
    return (0);
  tnode = node;
  while (node_down (tnode) != 0)
    tnode = node_down (tnode);
  return (tnode);
}

Node *
node_insert_up (Node * node)
{
  Node *temp, *new = node_alloc ();

  assert (node);

  temp = node->up;
  new->up = temp;
  new->down = node;

  node->up = new;
  if (temp != 0)
    temp->down = new;

  new->left = node->left;

  if(node_left(new)){    /* make tree consistant */
     temp=node_left(new);
	 temp->right=new;
  }

  return new;
}


Node *
node_insert_down (Node * node)
{
  Node *temp, *new = node_alloc ();

  assert (node);

  temp = node->down;
  new->down = temp;
  new->up = node;

  node->down = new;
  if (temp != 0)
    temp->up = new;

  new->left = node->left;

  return new;
}

Node *
node_insert_right (Node * node)
{
  Node *new = node_alloc ();

  if ((node == 0) || (node->right != 0))
    {
      free (new);
      return (0);
    };

  new->left = node;
  node->right = new;

  return new;
}

unsigned int
nodes_left (Node * node)
{
  unsigned int level = 0;
  while (node_left (node) != 0)
    {
      node = node_left (node);
      level++;
    };
  return (level);
}

unsigned int
nodes_up (Node * node)
{
  unsigned int level = 0;
  while (node_up (node) != 0)
    {
      node = node_up (node);
      level++;
    };
  return (level);
}

unsigned int
nodes_down (Node * node)
{
  unsigned int level = 0;
  while (node_down (node) != 0)
    {
      node = node_down (node);
      level++;
    };
  return (level);
}

unsigned int
nodes_right (Node * node)
{
  unsigned int level = 0;
  while (node_right (node) != 0)
    {
      node = node_right (node);
      level++;
    };
  return (level);
}

Node *
node_remove (Node * node)
{
  Node *tup, *tdown;

  assert (node);

  if (node == root)
    {
      if (root->down)
	root = root->down;
      else
	{
	  root = node_insert_down (root);
	  node_setflags (root, F_temp);
	};
    };

  while (node_right (node) != 0)
    node_remove (node_right (node));

  tup = node->up;
  tdown = node->down;

/*#warning    multiple places where node_free is called*/

  if (tup)
    {
      tup->down = tdown;
    };
  if (tdown)
    tdown->up = tup;

  if (node_left (node) != 0)
    {
      if (node_left (node)->right == node)
	{
	  if (tup)
	    node->left->right = tup;
	  else if (tdown)
	    node->left->right = tdown;
	  else
	    {Node *tnode=node_left(node);
	      node->left->right = 0;
	      node_free (node);
	      return (tnode);
	    }
	};
    };
  node_free (node);
  if (tup)
    return (tup);
  if (tdown)
    return (tdown);

  printf ("strange error removing\n");
  return (0);
}

Node *
node_match (char *match, Node * where)
{
/*
	Implemented as: the node at *where starts with *match -> return node
*/
  Node *node;

  node = node_top (where);
  if (strlen (match) == 0)
    return (0);

  while (node != 0)
    {
      if (strncmp (node->data, match, strlen (match)) == 0)
	return (node);
      node = node_down (node);
    };
  return (0);
}

Node *
node_exact_match (char *match, Node * where)
{
  Node *node;

  node = node_top (where);
  if (strlen (match) == 0)
    return (0);

  while (node != 0)
    {
      if (strcmp (node->data, match) == 0)
	return (node);
      node = node_down (node);
    };
  return (0);
}

/* this is a commodity funciton, and I didn't want to code it myself,.. I
searched the fine web, found, cut'd, 'n', pasted.. 
 url: http://www.brokersys.com/snippets/STRISTR.C
*/

/*
** Designation:  StriStr
**
** Call syntax:  char *stristr(char *String, char *Pattern)
**
** Description:  This function is an ANSI version of strstr() with
**               case insensitivity.
**
** Return item:  char *pointer if Pattern is found in String, else
**               pointer to 0
**
** Rev History:  07/04/95  Bob Stout  ANSI-fy
**               02/03/94  Fred Cole  Original
**
** Hereby donated to public domain.
*/

char *stristr(const char *String, const char *Pattern)
{
      char *pptr, *sptr, *start;
      int  slen, plen;       for (start = (char *)String,
           pptr  = (char *)Pattern,
           slen  = strlen(String),
           plen  = strlen(Pattern);            /* while string length not shorter than pattern length */            slen >= plen;            start++, slen--)
      {
            /* find start of pattern in string */
            while (toupper(*start) != toupper(*Pattern))
            {
                  start++;
                  slen--;                   /* if pattern longer than string */                   if (slen < plen)
                        return(NULL);
            }             sptr = start;
            pptr = (char *)Pattern;             while (toupper(*sptr) == toupper(*pptr))
            {
                  sptr++;
                  pptr++;                   /* if end of pattern then pattern was found */                   if ('\0' == *pptr)
                        return (start);
            }
      }
      return(NULL);
} 


/*
  returns the next recursive node having match as a substring, or 0 if not found.
  starting from where.
*/

Node *
node_recursive_match (char *match, Node * where)
{
  Node *tnode;

  tnode=where;
  if (strlen (match) == 0)
    return (0);

  tnode=node_recurse(tnode);  /* skip forward*/
  while (tnode != 0)
    {
      if (stristr (tnode->data, match ) != NULL)      /* case insensitive */
	return (tnode);
      tnode = node_recurse (tnode);
    };
  return (0);
}

Node *
tree_init ()
{

  if (root)
    {
      printf ("hey, trying to init root again!\n");
      return (root);
    };
  root = node_alloc ();
  node_setflags (root, F_temp);
  return (root);
}

Node *
tree_root ()
{
  return (root);
}

int tree_free(){
	if(root){
		while(node_down(tree_root())){
			node_remove(node_down(root));
		};
		node_remove(tree_root());
		node_free(tree_root());
		return(1);
	};
	return(0);
}

#define look into node_swap and wether it can be done easilier
#define swp(a,b,t)	t=a;a=b;b=t;

/*
   swaps all the data contained in the two specified node structures
*/

void
node_swap (Node * nodeA, Node * nodeB)
{
  /*
     swap: right, flags, data;    
   */
  Node *tnode;
  int tint;
  char *tstr;
  
  if(nodeA==root)root=nodeB;
     else if(nodeB==root)root=nodeA;

  swp (nodeA->right, nodeB->right, tnode);
  swp (nodeA->flags, nodeB->flags, tint);
  swp (nodeA->data, nodeB->data, tstr);

  if (node_right (nodeA))
    {
      tnode = node_right (nodeA);
      tnode = node_top (tnode);
      tnode->left = nodeA;
      while (node_down (tnode))
	{
	  tnode = node_down (tnode);
	  tnode->left = nodeA;
	};
    };

  if (node_right (nodeB))
    {
      tnode = node_right (nodeB);
      tnode = node_top (tnode);
      tnode->left = nodeB;
      while (node_down (tnode))
	{
	  tnode = node_down (tnode);
	  tnode->left = nodeB;
	};
    };
}

void node_update_parents_todo(Node *pos){
 	Node *tnode=node_top(pos);
	if(node_left(pos) && node_getflags(node_left(pos)) & F_todo ){
		int all_done=1;
		while(tnode!=0){
			if((node_getflags(tnode)&F_todo) && !(node_getflags(tnode)&F_done))all_done=0;
				tnode=node_down(tnode);
			};

		tnode=node_left(pos);
		if(all_done){
			   if( !(node_getflags(tnode)&F_done)){
			   	node_setflags( tnode,node_getflags(tnode)+F_done);
			   } 
		} else {
		   if( (node_getflags(tnode)&F_done)){
			   	node_setflags( tnode,node_getflags(tnode)-F_done);
		   } 
			
		}
	  node_update_parents_todo(tnode);
	}
}

/* bubble sorts the siblings of the current node, returns the same node as the 
prior */

Node *node_sort_siblings(Node *node){
	Node *pos;
	int pass,passes;
	int changes;
	int item;
	pos=node_top(node);

	passes=nodes_down(pos);

	for(pass=0;pass<passes;pass++){
		pos=node_top(pos);	
		changes=item=0;
		while(item<passes-pass){
			item++;
			if(strcmp(node_getdata(pos),node_getdata(node_down(pos)))>0){
			    if(pos==node){
				  node=node_down(pos);
				} else {
				  if(node_down(pos)==node)
				     node=pos;
				}  
				node_swap(pos,node_down(pos));
				changes++;
			};
			pos=node_down(pos);
		};
		if(!changes)return(node);
	};
	return(node);
}

/*
	returns an number between 0 and 1000 showing the completion of this node,
	computed as follows:
		all children with todo boxes count as one part
		if a child is done count+= 1000/parts
		if a child is not done count+= node_calc_complete(child)/parts	
*/

int node_calc_complete(Node *node){
	Node *tnode;int count=0,parts=0;

	if(!(node_getflags(node)&F_todo))return -1;		/* node has no completion status */
	if(node_getflags(node)&F_done)return 1000;		/* this node is done */
	if(!nodes_right(node))return 0;					/* no undone children,.. completly undone*/

	tnode=node_right(node);
	tnode=node_top(tnode);
	while(tnode){
		if(node_getflags(tnode)&F_todo)parts++;	
		tnode=node_down(tnode);
	}

	tnode=node_right(node);
	tnode=node_top(tnode);

	while(tnode){
		if(node_getflags(tnode)&F_todo){
			if(node_getflags(tnode)&F_done){
				count+= 1000/parts;
			} else {
				count+= node_calc_complete(tnode)/parts;			
			};
		}
		tnode=node_down(tnode);
	}
	return count;
}
