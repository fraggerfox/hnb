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

#include "tree.h"
#include "tree_sort.h"
#include "tree_todo.h"
#include "ui.h"
#include "file.h"

#include "prefs.h"

#include "ui_cli.h"
#include "cli.h"
#include "evilloop.h"

char inputbuf[BUFFERLENGTH];


int hnb_nodes_down;
int hnb_nodes_up;

const char *collapse_names[] = {
	"all (standard)",
	"all but first level of children",
	"show whole tree",
	"show path of current level",
	""
};

Node *evilloop (Node *pos)
{

#define chktemp		if (node_getflag(pos,F_temp)){\
					pos = node_remove (pos);\
					node_update_parents_todo (pos);\
					prevpos=pos;\
					prevpos=NULL;\
				}

#define chktemp2	if (node_getflag(pos,F_temp)){\
					pos = node_remove (pos);\
					node_update_parents_todo (pos);\
					prevpos=NULL;\
				}


	int stop = 0;
	Node *prevpos = pos;		/* which node we visited previous time */

	cli_outfun=set_status;

	while (!stop) {
		Tbinding *binding;

		ui_draw (pos, prevpos, inputbuf, 0);
		prevpos = pos;
		binding = parsekey (ui_input (), ui_current_scope);
		do{ 		

			switch (binding->action) {
			case ui_action_quit:
				stop=1;
				break;	
			case ui_action_command:
				pos=docmd(pos,binding->action_param);
				inputbuf[0]=0;
				prevpos = pos;
				break;
			case ui_action_top:
				chktemp;
				inputbuf[0] = 0;
				pos = node_root (pos);
				break;
			case ui_action_bottom:
				chktemp;
				inputbuf[0] = 0;
				pos = node_bottom (pos);
				break;
			case ui_action_up:
				chktemp
			else {
				if (node_up (pos))
					pos = node_up (pos);
				else if (prefs.forced_up) {
					if (node_left (pos))
						pos = node_left (pos);
				}
			}
				inputbuf[0] = 0;
				break;
			case ui_action_down:
				chktemp
				else
			if (node_down (pos))
				pos = node_down (pos);
			else if (prefs.forced_down) {
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
			case ui_action_pagedown:
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
				chktemp;
				inputbuf[0] = 0;
				{
					int n;

					for (n = 0; n < hnb_nodes_up; n++)
						if (node_up (pos))
							pos = node_up (pos);
				}
				break;
			case ui_action_left:
				chktemp2
			else {				/* FIXME: hva er problemet her?,.. noder forsvinner */
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
			case ui_action_cli:
				ui_end ();
				pos = cli (pos);
				prevpos = pos;
				ui_init ();
				break;
/*			case ui_action_size:
			if (node_getsize (pos) != -1)
					sprintf (inputbuf,
							 "Current size [%i], enter new (0.0-10.0) press return to keep",
							 node_getsize (pos));
				else
					sprintf (inputbuf,
							 "Current size [--], enter new (0.0-10.0) press return to keep");
				ui_draw (pos, pos, inputbuf, ui_mode_getstr);
				if (strlen (inputbuf)) {
					double p = atof (inputbuf);

					if (p > 0)
						node_setsize (pos, (int)(p*10.01));
					else
						node_setsize (pos, -1);
				}
				inputbuf[0] = 0;
				break;
			case ui_action_percentage:
			if (node_getpercent_done (pos) != -1)
					sprintf (inputbuf,
							 "Current percent_done [%i], enter new (-1,0..100) press return to keep",
							 node_getpercent_done (pos));
				else
					sprintf (inputbuf,
							 "Current percent_done [--], enter new (-1,0..100) press return to keep");
				ui_draw (pos, pos, inputbuf, ui_mode_getstr);
				if (strlen (inputbuf)) {
					int p = atoi (inputbuf);

					if (p >= -1 && p <= 100)
						node_setpercent_done (pos, p);
				}
				inputbuf[0] = 0;
				break;
*/
			case ui_action_backspace:
				if (!strlen (inputbuf)) {
					/*pos = ui_remove (pos);*/
				} else {
					inputbuf[strlen (inputbuf) - 1] = 0;
					if (node_getflag (pos, F_temp))
						if (node_up (pos))
							prevpos = pos = node_remove (pos);
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
