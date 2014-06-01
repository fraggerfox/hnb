/*
 * registry.c -- general functions for storing and retrieving tokens from a db
 *
 * Copyright (C) 2001,2001 Øyvind Kolås <pippin@users.sourceforge.net>
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


#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "path.h"
#include "registry.h"

Node *root;

char *reg_get (char *data)
{
	Node *pos;
	static char empty[2] = "";

	pos = path2node (data, root);

	if (pos)
		return (pos->data);
	return (empty);
}

int reg_get_int (char *path)
{
	Node *pos;

	pos = path2node (path, root);

	if (pos)
		return atoi (pos->data);
	return 0;
}

void reg_set (char *data)
{
	path2node_make (data, root);
}

void reg_set_int (char *path, int data)
{
	char buf[100];

	sprintf (buf, "%s%i", path, data);
	path2node_make (buf, root);
}

/*
void reg_load (char *registry){
	root = tree_new ();
	root = ascii_import (root, registry);
}

void reg_save (char *registry){
	ascii_export (root, registry);
}
*/
void reg_close ()
{
	tree_free (root);
}
