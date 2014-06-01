
/*
 * node.c -- definition of hnb's nodes
 *
 * Copyright (C) 2001-2003 Øyvind Kolås <pippin@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <stdlib.h>
#include "node.h"

char *node_setdata (Node *node, const char *data)
{
	free (node->data);
	node->data = (char *) strdup (data);
	return (node->data);
}

char *node_getdata (Node *node)
{
	static char empty[2] = "";

	if (node)
		return node->data;
	return (empty);
}

Node *node_new ()
{
	Node *node = (Node *) malloc (sizeof (Node));

	node->up = 0;
	node->down = 0;
	node->right = 0;
	node->left = 0;
	node->data = (char *) strdup ("");
	node->flags = 0;
	node->priority = 0;
	node->percent_done = -1;
	node->size=-1;	
	return node;
}

void node_free (Node *node)
{
	free (node->data);
	free (node);
}
