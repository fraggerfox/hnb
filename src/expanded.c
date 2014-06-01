/*
 * expanded.c -- functions to expand/unexpand nodes
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

#include "tree.h"
#include "cli.h"
#define NULL 0

static int cmd_expand(char *paramd, void *data){
	Node *pos=(Node *)data;
	pos->expanded=1;
	return (int)pos;
}

static int cmd_collapse(char *paramd, void *data){
	Node *pos=(Node *)data;
	pos->expanded=0;
	return (int)pos;
}

static int cmd_expand_all(char *paramd, void *data){
	Node *pos=(Node *)data;
	Node *tnode=node_root(pos);
	while(tnode){
		tnode->expanded=1;
		tnode=node_recurse(tnode);
	}
	cli_outfun("expanded all nodes");
	return (int)pos;
}

static int cmd_collapse_all(char *paramd, void *data){
	Node *pos=(Node *)data;
	Node *tnode=node_root(pos);
	while(tnode){
		tnode->expanded=0;
		tnode=node_recurse(tnode);
	}
	cli_outfun("collapsed all nodes");
	return (int)pos;
}

/*
!init_expanded();
*/
void init_expanded(){
		cli_add_command ("expand", cmd_expand, "");
		cli_add_help("expand","expand the current node, thus showing it's subnodes");
		cli_add_command ("collapse", cmd_collapse, "");
		cli_add_help("collapse","collapse the current node's subnodes");

		cli_add_command ("expand_all", cmd_expand_all, "");
		cli_add_help("expand_all","expand all expandable nodes in the tree");
		cli_add_command ("collapse_all", cmd_collapse_all, "");
		cli_add_help("collapse_all","collapses all expanded nodes in the tree");
}
