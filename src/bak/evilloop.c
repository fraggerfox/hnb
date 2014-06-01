/*
 * evilloop.c -- The event loop / heart of execution for hnb
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


#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "tree_todo.h"
#include "ui.h"
#include "file.h"

#include "prefs.h"

#include "ui_cli.h"
#include "cli.h"
#include "evilloop.h"

char inputbuf[BUFFERLENGTH];

static int forced_up=0;
static int forced_down=0;

int hnb_nodes_down;
int hnb_nodes_up;

const char *collapse_names[] = {
	"all (standard)",
	"all but first level of children",
	"show whole tree",
	"show path of current level",
	""
};

/*  removes *pos if it is a temporary node, then returns 1
 *  otherwize returns 0
 */
static int remove_temp(Node **pos){
	if (node_getflag(*pos,F_temp)){
		*pos = node_remove ((*pos));
		node_update_parents_todo ((*pos));
		return 1;
	}
	return 0;
}

Node *evilloop (Node *pos)
{
	int stop = 0;

	cli_outfun=set_status;

	while (!stop) {
		Tbinding *binding;

		ui_draw (pos, inputbuf, 0);
		binding = parsekey (ui_input (), ui_current_scope);
		do{ 		

			switch (binding->action) {
			case ui_action_quit:
				remove_temp(&pos);
				stop=1;
				break;	
			case ui_action_command:
				if(strcmp(binding->action_param,"edit"))
					remove_temp(&pos);
				pos=docmd(pos,binding->action_param);
				inputbuf[0]=0;
				break;
			case ui_action_top:
				remove_temp(&pos);
				inputbuf[0] = 0;
				pos = node_top (pos);
				break;
			case ui_action_bottom:
				remove_temp(&pos);			
				inputbuf[0] = 0;
				pos = node_bottom (pos);
				break;
			case ui_action_up:
				if(!remove_temp(&pos)){
					if (node_up (pos))
						pos = node_up (pos);
					else if (forced_up) {
						if (node_left (pos))
							pos = node_left (pos);
					}
				}
				inputbuf[0] = 0;
				break;
			case ui_action_down:
				if(!remove_temp(&pos)){
					if (node_down (pos)){
						pos = node_down (pos);
					} else if (forced_down) {
						while (node_left (pos)) {
							if (node_down (pos)) {
								break;
							}
							pos = node_left (pos);
						}
						if (node_down (pos))
							pos = node_down (pos);
					}
					inputbuf[0] = 0;
					break;
				}
			case ui_action_pagedown:
				remove_temp(&pos);
				inputbuf[0] = 0;
				{
					int n;

					for (n = 0; n < hnb_nodes_down; n++)
						if (node_down (pos)) {
							pos = node_down (pos);
						}
				}
				break;
			case ui_action_pageup:
				remove_temp(&pos);
				inputbuf[0] = 0;
				{
					int n;

					for (n = 0; n < hnb_nodes_up; n++)
						if (node_up (pos))
							pos = node_up (pos);
				}
				break;
			case ui_action_left:
				if(!remove_temp(&pos)){
					if (node_left (pos))
						pos = node_left (pos);
				}
				inputbuf[0] = 0;
				break;
			case ui_action_right:
				if (node_right (pos)) {
					pos = node_right (pos);
				} else {
					if (node_getdata (pos)[0]) {
						node_insert_right (pos);
						if (node_getflag (pos, F_temp))
							node_setflag (pos, F_temp, 0);
						if (node_getflag (pos, F_todo))
							node_setflag (node_right (pos), F_todo, 1);
						node_setflag (node_right (pos), F_temp, 1);
						pos = node_right (pos);
					}
				}
				inputbuf[0] = 0;
				break;
			case ui_action_complete:
				if (strcmp (inputbuf, node_getdata (pos)) == 0) {
					if (node_right (pos)) {
						pos = node_right (pos);
					} else {
						if (node_getdata (pos)[0]) {
							node_insert_right (pos);
							if (node_getflag (pos, F_temp))
								node_setflag (pos, F_temp, 0);
							if (node_getflag (pos, F_todo))
								node_setflag (node_right (pos), F_todo, 1);
							node_setflag (node_right (pos), F_temp, 1);

							pos = node_right (pos);
						}
					}
					inputbuf[0] = 0;
				} else {
					strcpy (inputbuf, node_getdata (pos));
				}
				break;
			case ui_action_cancel:
				if (node_getflag (pos, F_temp)) {
					pos = node_remove (pos);
				} else {
					/*stop = ui_quit (pos);*/
				}
				inputbuf[0] = 0;
				break;
			case ui_action_backspace:
				if (!strlen (inputbuf)) {
					/*pos = ui_remove (pos);*/
				} else {
					inputbuf[strlen (inputbuf) - 1] = 0;
					if (node_getflag (pos, F_temp))
						if (node_up (pos))
							pos = node_remove (pos);
				}
				break;
			case ui_action_unbound:
				undefined_key (ui_scope_names[ui_current_scope], binding->key!=1000?binding->key:*((int*)&binding->action_param[0]) );
			case ui_action_ignore:
				break;
			default:
				if (binding->action > 31 && binding->action < 255) {	/*  input for buffer */
					inputbuf[strlen (inputbuf) + 1] = 0;
					inputbuf[strlen (inputbuf)] = binding->action;
				} else
					undefined_key (ui_scope_names[ui_current_scope], binding->key!=1000?binding->key:*((int*)&binding->action_param[0]) );
				break;
		} 
		}while((++binding)->key==999);
		
		
		if (strlen (inputbuf)) {
			if (node_getflag (pos, F_temp)) {
				node_setdata (pos, inputbuf);
			} else {
				if (node_match (inputbuf, pos)) {
					if (strcmp
						(node_getdata (node_match (inputbuf, pos)),
						 node_getdata (pos)) != 0)
						pos = node_match (inputbuf, pos);
				} else {
					pos = node_insert_down (node_bottom (pos));
					node_setflag (pos, F_temp, 1);
					node_setdata (pos, inputbuf);
					if (node_left (pos))
						if (node_getflag (node_left (pos), F_todo))
							node_setflag (pos, F_todo, 1);
				}
			}
		}
	}
	return pos;
}

/*
!init_evilloop();
*/

void init_evilloop(){
	cli_add_int("forced_up",  &forced_up,  "wether movement upwards is forced beyond first sibling" );
	cli_add_int("forced_down",&forced_down,"wether movement downwards is forced beyond last sibling" );
}
