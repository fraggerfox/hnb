/*
 * tree_misc.c -- various functions bindable/callable from hnb, should be seperated
 *                out into seperate files
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
#include "node.h"
#include "prefs.h"
#include "ui.h"
#include "ui_cli.h"
#include "evilloop.h"
/*
!cli cli_add_command ("movenode", cmd_movenode, "<up|left|right|down>");
!clid int cmd_movenode ();
*/

int cmd_movenode(char *params,void *data){
	Node *pos=(Node *)data;
	if(!strcmp(params,"left")){	
		if(node_left(pos)){
			Node *tnode, *bnode;
			bnode=pos;
			tnode = node_insert_down (node_left (pos));
			node_swap (tnode, bnode);
			node_remove (bnode);
			pos=tnode;
		}
	} else if(!strcmp(params,"right")){	
		if(node_up(pos)){
			Node *tnode;
					
			pos=node_up(pos);
			if(!(tnode=node_right(pos))){
				tnode=node_insert_right(pos);
			} else {
				tnode=node_insert_down(node_bottom(tnode));
			}
			node_swap(node_down(pos),tnode);
			node_remove(node_down(pos));
			pos=tnode;
		}
	} else if(!strcmp(params,"up")){	
		if(node_up(pos)){
			node_swap(pos,node_up(pos));
			pos=node_up(pos);
		}
	} else if(!strcmp(params,"down")){	
		if(node_down(pos)){
			node_swap(pos,node_down(pos));
			pos=node_down(pos);
		}
	}
	return (int)pos;
}

/*
!cli cli_add_command ("save", cmd_save, "");
!clid int cmd_save ();
*/

int cmd_save(char *params,void *data){
	Node *pos=(Node *)data;
					if (prefs.db_file[0] != (char) 255) {
					int error = 0;	/* FIXME: get status,. */

					{
						char buf[4096];

						if (prefs.format == format_hnb) {
							sprintf (buf, "export_%s %s %i",
									 format_name[prefs.format], prefs.db_file,
									 node_no (pos) - 1);
						} else {
							sprintf (buf, "export_%s %s",
									 format_name[prefs.format],
									 prefs.db_file);
						}
						docmd (node_root (pos), buf);
					}

					if (!error) {
						docmdf (pos,"status wrote stuff to '%s'", prefs.db_file);
					} else {
						docmdf (pos,"status failed to open '%s' for writing",   prefs.db_file);
					}
				}
	return (int)pos;
}

/*
!cli cli_add_command ("outdent", cmd_outdent, "");
!clid int cmd_outdent ();
*/


int cmd_outdent (char *params, void *data)
{
	Node *pos=(Node *)data;

	if (node_left (pos)) {
		Node *tnode, *bnode;

		bnode = node_bottom (pos);
		while (node_down (pos)) {
			tnode = node_insert_down (node_left (pos));
			node_swap (tnode, bnode);
			bnode = node_remove (bnode);
		}
		tnode = node_insert_down (node_left (pos));
		node_swap (tnode, bnode);
		node_remove (bnode);
		return (int)tnode;
	}
	return (int)pos;
}

/*
!cli cli_add_command ("indent", cmd_indent, "");
!clid int cmd_indent ();
*/

int cmd_indent (char *params, void *data)
{
	Node *pos=(Node *)data;
	if (node_up (pos)) {
		Node *tnode, *first_moved;

		pos = node_up (pos);	/* go up  */

		if (!(tnode = node_right (pos))) {	/* must install a child */
			tnode = node_insert_right (pos);
		} else {
			tnode = node_insert_down (node_bottom (tnode));
		}

		node_swap (node_down (pos), tnode);
		node_remove (node_down (pos));
		first_moved = tnode;
		while (node_down (pos)) {
			tnode = node_insert_down (tnode);
			node_swap (node_down (pos), tnode);
			node_remove (node_down (pos));
		}
		return (int)first_moved;
	}
	return (int)pos;
}


/*
!cli cli_add_command ("toggle_todo", toggle_todo_cmd, "");
!cli cli_add_command ("toggle_done", toggle_done_cmd, "");
!clid int toggle_todo_cmd ();
!clid int toggle_done_cmd ();
*/

int toggle_todo_cmd(char *params,int data){
	Node *pos=(Node *)data;
	node_toggleflag (pos, F_todo);
	return (int)pos;
}

int toggle_done_cmd(char *params,int data){
	Node *pos=(Node *)data;
	node_toggleflag (pos, F_done);
	return (int)pos;
}

/*
!cli cli_add_command ("remove", remove_cmd, "");
!clid int remove_cmd ();
*/


int remove_cmd(char *params,int data){
	Node *pos=(Node *)data;
	if (node_right (pos)) {
		Tbinding *c;

		docmdf (pos, "status node has children, really remove?");
		ui_draw(pos,pos,"",0 );		
		c = parsekey (ui_input (), ui_scope_confirm);
		if (c->action == ui_action_confirm )
			pos = node_remove (pos);
	} else {
		pos = node_remove (pos);
	}
	return (int)pos;
}

/*
!cli cli_add_command ("commandline", commandline_cmd, "");
!clid int commandline_cmd ();
*/

int commandline_cmd (char *params, void *data){
	char commandline[80];
	Node *pos=(Node *)data;

	do{
		strcpy(commandline,"");
		ui_draw(pos,pos,"",0);		
		ui_getstr("commandline interface, enter blank command to cancel",commandline);

		if(commandline[0])
			pos=docmd(pos,commandline);
	}while(commandline[0]);
	return (int)pos;
}

/*
!cli cli_add_command ("insert_below", insert_below_cmd, "");
!clid int insert_below_cmd ();
*/

int insert_below_cmd (char *params, void *data)
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


/************** search ************************

!cli cli_add_command ("pushd", pushd_cmd, "");
!cli cli_add_command ("popd", popd_cmd, "");
!cli cli_add_command ("prev_match", prev_match_cmd, "");
!cli cli_add_command ("getquery", getquery_cmd, "");
!cli cli_add_command ("next_match", next_match_cmd, "");
!clid int next_match_cmd ();
!clid int prev_match_cmd ();
!clid int pushd_cmd ();
!clid int popd_cmd ();
!clid int getquery_cmd ();
*/


/*FIXME: implement real stack*/
static Node *stack_saved=NULL;

int pushd_cmd(char *params,int data){
	Node *pos=(Node *)data;
	stack_saved=pos;
	return (int)pos;
}

int popd_cmd(char *params,int data){
	Node *pos=(Node *)data;
	if(stack_saved != NULL){
		pos=stack_saved;
		stack_saved=NULL;
	}
	return (int)pos;
}


int next_match_cmd(char *params,int data){
	Node *pos=(Node *)data;
	pos = node_recursive_match ((char *) prefs.query, pos);

	if(pos==NULL){
		docmdf(pos,"status reached bottom of tree and '%s' not found", prefs.query);
		return (int)data;
	}

	/* might evoke a message of wrapping the tree,.. check startlevel as well? */

	return (int)pos;
}



int prev_match_cmd(char *params,int data){
	Node *pos=(Node *)data;
	pos = node_backrecursive_match ((char *) prefs.query, pos);


	if(pos==NULL){
		docmdf(pos,"status reached top of tree and '%s' not found", prefs.query);
		return (int)data;
	}

	/* might evoke a message of wrapping the tree,.. check startlevel as well? */

	return (int)pos;
}


int getquery_cmd(char *params,int data){
	Node *pos=(Node *)data;
	strcpy(prefs.query,"");
	ui_getstr("Search for:",&prefs.query[0]);
	return (int)pos;
}


/************************* clipboard ************************************/

#include "file.h"

/*
!cli cli_add_command ("copy", copy_cmd, "");
!cli cli_add_command ("cut", cut_cmd, "");
!cli cli_add_command ("paste", paste_cmd, "");
!clid int copy_cmd ();
!clid int cut_cmd ();
!clid int paste_cmd ();
*/

static void clipboard_duplicate_tree(Node *source, Node *target){
	int level, flags, priority, startlevel;
	import_state_t ist;	
	char *data;
		
	node_setflags (target, node_getflags (source));
	node_setpriority(target, node_getpriority(source));
	node_setdata (target, node_getdata (source));

	init_import (&ist, target);
	
	if (node_right (source)) {
		source = node_right (source);
		startlevel = nodes_left (source);
		while ((source != 0) & (nodes_left (source) >= startlevel)) {
			level = nodes_left (source) - startlevel + 1;
			flags = node_getflags (source);
			priority = node_getpriority(source);
			data = node_getdata (source);

			import_node (&ist, level, flags, priority, data);
			source = node_recurse (source);
		}
	}
}

static Node *clipboard=NULL;

int copy_cmd (char *params, void *data)
{
	Node *pos=(Node *)data;
	if(clipboard!=NULL){
		tree_free(clipboard);
	}
	clipboard=node_new();

	clipboard_duplicate_tree( pos, clipboard);
	return (int)pos;
}

int cut_cmd (char *params, void *data)
{
	Node *pos=(Node *)data;
	if(clipboard!=NULL){
		tree_free(clipboard);
	}
	clipboard=node_new();

	clipboard_duplicate_tree( pos, clipboard);
	pos=node_remove(pos);
	return (int)pos;
}

int paste_cmd (char *params, void *data)
{
	Node *pos=(Node *)data;
	if(clipboard==NULL){
		docmd(pos,"status no data in clipboard");
	} else {
		Node *temp;
		temp=node_insert_down(pos);
		clipboard_duplicate_tree( clipboard, temp);
	}
	return (int)pos;
}




/*
	TODO:
		copy
		paste
		cut
		setting of attributes,.. percentage, size, donebydate etc.
*/
