/*
 * search.c -- recursive search for hnb
 *             
 *
 * Copyright (C) 2001,2003 Øyvind Kolås <pippin@users.sourceforge.net>
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
#include "prefs.h"
#include "ui.h"
#include <cli.h>

/************** search ************************/

static int next_match_cmd(char *params,void *data){
	Node *pos=(Node *)data;
	pos = node_recursive_match ((char *) prefs.query, pos);

	if(pos==NULL){
		docmdf(pos,"status reached bottom of tree and '%s' not found", prefs.query);
		return (int)data;
	}
	return (int)pos;
}

static int prev_match_cmd(char *params,void *data){
	Node *pos=(Node *)data;
	pos = node_backrecursive_match ((char *) prefs.query, pos);

	if(pos==NULL){
		docmdf(pos,"status reached top of tree and '%s' not found", prefs.query);
		return (int)data;
	}
	return (int)pos;
}

static int getquery_cmd(char *params,void *data){
	Node *pos=(Node *)data;
	strcpy(prefs.query,"");
	ui_getstr("Search for:",&prefs.query[0]);
	return (int)pos;
}
/*
!init_search();
*/
void init_search(){
	cli_add_command ("getquery", getquery_cmd, "");
	cli_add_help("getquery","Provides the user with a dialog asking him for a search term.");
	cli_add_command ("prev_match", prev_match_cmd, "");
	cli_add_help("prev_match","Moves backwards in the tree to the prior match");
	cli_add_command ("next_match", next_match_cmd, "");
	cli_add_help("next_match","Moves forward in the tree to the next match");
}
