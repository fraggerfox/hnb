#include <string.h>
#include "node.h"
#include "tree.h"
#include "file.h"
#include "path.h"

Node *root;

char *reg_get (char *data)
{
	Node *pos;

	pos = path2node (data, root);

	if (pos)
		return (pos->data);
	return ("");

}

void reg_set (char *data)
{
	path2node_make (data, root);
}

void reg_load (char *registry)
{
	root = tree_new ();
	root = ascii_import (root, registry);
}

void reg_save (char *registry)
{
	ascii_export (root, registry);
}

void reg_close ()
{
	tree_free (root);
}
