#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"

void
node_setdata (Node * node, char *data)
{
  assert (node);
  free (node->data);
  node->data = (char *) strdup (data);
}

char *
node_getdata (Node * node)
{
  assert (node);
  return node->data;
}

Node *
node_alloc ()
{
  Node *node = (Node *) malloc (sizeof (Node));
  node->up = 0;
  node->down = 0;
  node->right = 0;
  node->left = 0;
  node->data = (char *) strdup ("");
  node->flags = 0;
  return node;
}

void
node_free (Node * node)
{
  free (node->data);
  free (node);
}
