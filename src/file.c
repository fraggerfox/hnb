/*
 * file.c -- utility functions for import/export hnb
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


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "tree.h"
#include "file.h"

/**i string used to return error state from import/export functions
 *
 * */
char file_error[128] = "";

void init_import (import_state_t * is, Node *node)
{
	is->npos = node;
	is->startlevel = nodes_left (node);
}

/*
 *
 * @return the node inserted
 * */
Node *import_node (import_state_t * is, int level, int flags,
				  int priority, char *data)
{
	int node_level;

	level += is->startlevel;

	while ((node_level = nodes_left (is->npos)) > level)
		is->npos = node_left (is->npos);
	if (node_level == level)
		is->npos = node_insert_down (is->npos);
	if (node_level < level)
		is->npos = node_insert_right (is->npos);
	node_setflags (is->npos, flags);
	node_setpriority (is->npos, priority);
	node_setdata (is->npos, data);
	return is->npos;
/*	node_update_parents_todo(is->npos); commented out due to major slowdown
when importing */
}
