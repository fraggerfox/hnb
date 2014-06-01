/*
 * tree_misc.c -- various functions bindable/callable from hnb, should be seperated
 *                out into seperate files
 *
 * Copyright (C) 2001,2003 �yvind Kol�s <pippin@users.sourceforge.net>
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
#include "tree.h"
#include "node.h"
#include "prefs.h"
#include "ui.h"
#include "ui_cli.h"
#include "evilloop.h"

static int cmd_movenode(char *params,void *data){
	Node *pos=(Node *)data;
	if(!strcmp(params,"left")){	
		if(node_left(pos)){
			Node *tnode;
			tnode = node_insert_down(node_left(pos));
			node_swap(pos,tnode);
			node_remove(tnode);
		}
	} else if(!strcmp(params,"right")){	
		if(node_up(pos)){
			Node *tnode;
					
			tnode=node_up(pos);
			if(!node_right(tnode)){
				tnode=node_insert_right(tnode);
			} else {
				tnode=node_insert_down(node_bottom(node_right(tnode)));
			}
			node_swap(pos,tnode);
			node_remove(tnode);
		}
	} else if(!strcmp(params,"up")){	
		if(node_up(pos)){
			node_swap(pos,node_up(pos));
		}
	} else if(!strcmp(params,"down")){	
		if(node_down(pos)){
			node_swap(pos,node_down(pos));
		}
	}
	return (int)pos;
}

/*
!init_movenode();
*/
void init_movenode(){
	cli_add_command ("movenode", cmd_movenode, "<up|left|right|down>");
}

static int cmd_go_root(char *params,void *data){
	if(node_backrecurse((Node*)data))
		return (int)node_root((Node*)data);
	return (int)data;
}

static int cmd_go_recurse(char *params,void *data){
	if(node_recurse((Node*)data))
		return (int)node_recurse((Node*)data);
	return (int)data;
}

static int cmd_go_backrecurse(char *params,void *data){
	return (int)node_backrecurse((Node*)data);
}


/*
!init_go_root();
*/
void init_go_root(){
	cli_add_command ("go_root", cmd_go_root, "");
	cli_add_help("go_root","skip to the root of the tree");
	cli_add_command ("go_recurse", cmd_go_recurse, "");
	cli_add_help("go_recurse","go to the next node recursively");
	cli_add_command ("go_backrecurse", cmd_go_backrecurse, "");
	cli_add_help("go_backrecurse","go to the previous node recursively");

}

static int cmd_outdent (char *params, void *data)
{
	Node *pos=(Node *)data;

	if (node_left (pos)) {
		Node *target_node=node_left(pos);
		Node *last_node=node_bottom(pos);
		Node *first_node=pos;
		Node *prev_up=node_up(pos);
		Node *prev_target_down=node_down(target_node);
		Node *tnode;
		
		tnode=first_node;
		while(tnode!=last_node){
			tnode->left=target_node->left;
			tnode=tnode->down;
		};
		tnode->left=target_node->left;
		
		first_node->up=target_node;
		target_node->down=first_node;
		last_node->down=prev_target_down;

		if(prev_target_down)
			prev_target_down->up=last_node;
		
		if(prev_up){
			prev_up->down=NULL;
		} else {
			target_node->right=NULL;
		}		
	}
	return (int)pos;
}

/*	FIXME: no real need for a temporary node */

static int cmd_indent (char *params, void *data)
{
	Node *pos=(Node *)data;
	if(node_up(pos)){
		Node *tnode, *snode;

		tnode=node_up(pos);
		if(!node_right(tnode)){
			tnode=node_insert_right(tnode);
		} else {
			tnode=node_insert_down(node_bottom(node_right(tnode)));
		}
		node_swap(pos,tnode);
		snode=tnode;
		
		node_up(snode)->down=NULL;
		snode->up=pos;
		pos->down=snode;

		while(snode){
			snode->left=pos->left;
			snode=node_down(snode);
		}
		node_remove(node_down(pos));
	}
	return (int)pos;
}
/*
!init_outdent_indent();
*/
void init_outdent_indent(){
	cli_add_command ("outdent", cmd_outdent, "");
	cli_add_help("outdent","moves the active item and the following siblings one level to the left");
	cli_add_command ("indent", cmd_indent, "");
	cli_add_help("indent","moves the active item and the following siblings one level to the right");
}

static int remove_cmd(char *params,void *data){
	Node *pos=(Node *)data;
	if (node_right (pos)) {
		Tbinding *c;
		int tempscope=ui_current_scope;
		ui_current_scope=ui_scope_confirm;
		docmdf (pos, "status node has children, really remove?");
		ui_draw(pos,"",0 );
		ui_current_scope=tempscope;
		c = parsekey (ui_input (), ui_scope_confirm);
		if (c->action == ui_action_confirm ){
			docmd(pos,"save_state");
			pos = node_remove (pos);
		}
	} else {
		docmd(pos,"save_state");
		pos = node_remove (pos);
	}
	return (int)pos;
}
/*
!init_remove();
*/
void init_remove(){
	cli_add_command ("remove", remove_cmd, "");
	cli_add_help("remove","Removes the active node, if it has children a confirmation dialog pops up.");
}


static int commandline_cmd (char *params, void *data){
	Node *pos=(Node *)data;

	char commandline[80];

	do{
		strcpy(commandline,"");
		ui_draw(pos,"",0);		
		ui_getstr("commandline interface, enter blank command to cancel",commandline);

		if(commandline[0])
			pos=docmd(pos,commandline);
	}while(commandline[0]);
	return (int)pos;
}
/*
!init_commandline();
*/
void init_commandline(){
	cli_add_command ("commandline", commandline_cmd, "");
	cli_add_help("commandline","Invokes the interactive commandline in curses mode.");
}

static int insert_below_cmd (char *params, void *data)
{
	Node *pos=(Node *)data;

	if (node_getflag (pos, F_temp)) {
		node_toggleflag (pos, F_temp);
	} else {
		pos = node_insert_down (pos);
		if (node_left (pos))
			if (node_getflag (node_left (pos), F_todo))
			node_setflag (pos, F_todo, 1);
	}
	inputbuf[0] = 0;
	return (int)pos;
}
/*
!init_insertbelow();
*/
void init_insertbelow(){
	cli_add_command ("insert_below", insert_below_cmd, "");
	cli_add_help ("insert_below","Adds a new node immediatly below the active");
}

/*
	TODO:
		setting of attributes,.. percentage, size, donebydate etc.
*/
