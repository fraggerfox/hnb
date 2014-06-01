#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"
#include "tree.h"

static Node *root = 0;

Node *
node_recurse (Node * node)
{

  if (node_right (node))
    return (node_right (node));
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
  Node *tup, *tdown, tnode;

  assert (node);
  tnode = *node;

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

/* FIXME: 
	multiple places where node_free is called,
	tnode is not used (much)
*/

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
	    {
	      node->left->right = 0;
	      node_free (node);
	      return (tnode.left);
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
node_recursive_match (char *match, Node * where)
{
/*
	Implemented as: the node at *where starts with *match -> return node
*/
  int startlevel = nodes_left (where);

  Node *node;

  node = node_top (where);
  if (strlen (match) == 0)
    return (0);

  while (node != 0)
    {
      if (strncmp (node->data, match, strlen (match)) == 0)
	return (node);
      node = node_recurse (node);
      if (nodes_left (node) < startlevel)
	node = 0;
    };
  return (0);
}


Node *
tree_match (char *match, Node * where)
{
/*
	translate match into a node,.. should also
	be used for links..
	
	/ = parent/child seperator and root identifier
	\ = escape (for escaping \ and / )

	if start = /, .. where=root
		strip first char store rest,
				
while rest contains /
		
	head = up to first /, store rest
	head = head - "/"
	
	if head != legal match 
		if match="" where=match("")
			else
				go bottom, insert_down, where=bottom
		setdata where=head
			
	if not nodes_right 
		insert right
	where=right
	
next while

	head=rest

	if head != legal match 
	if match="" where=match("")
		else
			go bottom, insert_down, where=bottom
	setdata where=head

	highlight matching chars
		
*/
  Node *node;

  if (match[0] == '/')
    {
      memcpy (&match[0], &match[1], 99);
      return (root);
    };

  node = node_top (where);

  while (node != 0)
    {
      if (strncmp (node->data, match, strlen (match)) == 0)
	return (node);
      node = node_down (node);
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


#define swp(a,b,t)	t=a;a=b;b=t;

void
node_swap (Node * nodeA, Node * nodeB)
{
  /*
     swap: right, flags, data;    
   */
  Node *tnode;
  int tint;
  char *tstr;

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
