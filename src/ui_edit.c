/*
 * ui_edit.c -- lineeditor/(readline like stuff) and bindings side of nodecontent editor
 *
 * Copyright (C) 2001-2003 �yvind Kol�s <pippin@users.sourceforge.net>
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
 
 #include <ctype.h>
#include <string.h>

#include "tree.h"
#include "ui.h"
#include "prefs.h"
#include "ui_style.h"
#include "ui_overlay.h"
#include "evilloop.h"
#include <stdlib.h>

/*
!cli cli_add_command ("edit", ui_edit_cmd, "");
!clid int ui_edit_cmd ();
*/

int ui_edit_cmd (char *params, void *data)
{
	Tbinding *c;
	int stop = 0;
	static int cursor_pos;
	static char *data_backup;
	static char input[BUFFERLENGTH];
	Node *pos=(Node *)data;

	memset (input, 0, sizeof (input));

	if(inputbuf[0]){	/* there is data in the inputbuffer,.. 
	                       we should not edit, but handle that
						   data instead...  this doesn't really belong here,.. but the hack works
	                    */
		if (node_getflag (pos, F_temp)) {
			node_setflag (pos, F_temp, 0);
		} else {
			pos = node_insert_down (node_bottom (pos));
			node_setdata (pos, input);
			if (node_getflag (node_left (pos), F_todo)) {
				node_setflag (pos, F_todo, 1);
			}
		}
		return (int)pos;
	}

/* FIXME: it shouldn't reference the node structure directly */

	data_backup = pos->data;
	data_backup = node_getdata (pos);
	input[0] = 0;
	strcpy (&input[0], data_backup);
	pos->data = &input[0];
	cursor_pos = strlen (input);
	input[cursor_pos] = ' ';
	input[cursor_pos + 1] = 0;
	input[cursor_pos + 2] = 0;

	while (!stop) {
		ui_draw (pos, pos, (char *) cursor_pos, 1);
		c = parsekey (ui_input (), ui_scope_nodeedit);
		switch (c->action) {
			case ui_action_right:
				if (cursor_pos < (strlen (input) - 1))
					cursor_pos++;
				break;
			case ui_action_left:
				if (cursor_pos)
					cursor_pos--;
				break;
			case ui_action_skipword:
				if (cursor_pos < (strlen (input) - 1))
					cursor_pos++;
				while ((cursor_pos < (strlen (input) - 1))
					   && isalpha (input[cursor_pos]))
					cursor_pos++;
				while ((cursor_pos < (strlen (input) - 1))
					   && !isalpha (input[cursor_pos]))
					cursor_pos++;
				break;
			case ui_action_bskipword:
				if (cursor_pos)
					cursor_pos--;
				while (cursor_pos && !isalpha (input[cursor_pos]))
					cursor_pos--;
				while ((cursor_pos - 1) && isalpha (input[cursor_pos - 1]))
					cursor_pos--;
				if (isalpha (input[cursor_pos - 1]))
					cursor_pos--;
				break;
			case ui_action_kill_line:
				cursor_pos = 0;
				input[cursor_pos] = ' ';
				input[cursor_pos + 1] = 0;
				input[cursor_pos + 2] = 0;
				break;
			case ui_action_bol:
				cursor_pos = 0;
				break;
			case ui_action_eol:
				cursor_pos = strlen (input) - 1;
				break;
/*			case k_edit_kill_to_eol:
				input[cursor_pos] = ' ';
				input[cursor_pos +1]=0;
				input[cursor_pos +2]=0;				
																			break;
*/ case ui_action_cancel:
				strcpy (&input[0], data_backup);
				pos->data = &input[0];
				cursor_pos = strlen (input);
				input[cursor_pos] = ' ';
				input[cursor_pos + 1] = 0;
				stop = 1;
				break;
			case ui_action_up:
				if (hnb_edit_posup >= 0)
					cursor_pos = hnb_edit_posup;
				break;
			case ui_action_down:
				if (hnb_edit_posdown < strlen (input))
					cursor_pos = hnb_edit_posdown;
				else
					cursor_pos = strlen (input) - 1;
				break;
			case ui_action_confirm:
				stop = 1;
				break;
			case ui_action_delete:
				if (cursor_pos < (strlen (input) - 1)) {
					cursor_pos++;
					if (cursor_pos) {
						memmove (&input[cursor_pos - 1],
								 &input[cursor_pos],
								 strlen (input) - cursor_pos);
						input[strlen (input) - 1] = 0;
						cursor_pos--;
					}
				}
				break;
			case ui_action_backspace:
				if (cursor_pos) {
					memmove (&input[cursor_pos - 1],
							 &input[cursor_pos], strlen (input) - cursor_pos);
					input[strlen (input) - 1] = 0;
					cursor_pos--;
				}
				break;
			case ui_action_split:
					input[strlen (input) - 1] = 0;
					node_insert_down (pos);
					if (input[cursor_pos] == ' ')
						node_setdata (node_down (pos),
									  &input[cursor_pos + 1]);
					else
						node_setdata (node_down (pos), &input[cursor_pos]);
					input[cursor_pos] = ' ';
					input[cursor_pos + 1] = 0;
				break;
			case ui_action_join:
					if (node_down (pos)) {
						cursor_pos = strlen (input);
						strcpy (&input[cursor_pos - 1],
								node_getdata (node_down (pos)));
						input[strlen (input)] = ' ';
						input[strlen (input) + 1] = 0;
						if (node_right (node_down (pos))) {
							docmd(pos,"status refusing to remove node after join, because it has children");
						} else {
							node_remove (node_down (pos));
						}
						cursor_pos--;
					}
				break;
			case ui_action_unbound:
				undefined_key (ui_scope_names[ui_scope_nodeedit], c->key!=1000?c->key:*((int*)&c->action_param[0]) );
			case ui_action_ignore:
				break;				
			default:
				if (c->action > 31 && c->action < 255) {	/*  input for buffer */
					memmove (&input[cursor_pos + 1],
							 &input[cursor_pos],
							 strlen (input) - cursor_pos + 1);
					input[cursor_pos++] = c->action;
				}  else {
					undefined_key (ui_scope_names[ui_scope_nodeedit], c->key!=1000?c->key:*((int*)&c->action_param[0]) );
				}
				break;
		}
	}

	input[strlen (input) - 1] = 0;

	pos->data = data_backup;
	node_setdata (pos, input);

	return (int)data;
}

int ui_getstr_loc(char *input, int x,int y, int maxlen){
		int stop=0;
		Tbinding *c;
		int cursor_pos=strlen(input);
		char *data_backup=strdup(input);

		input[cursor_pos] = ' ';
		input[cursor_pos + 1] = 0;
		input[cursor_pos + 2] = 0;


		if(x==-1 && y==-1){
			getyx(stdscr,y,x);
		}
		
		while (!stop) {
		
		move(y,x);
		clrtoeol();
		move(y,x);

		ui_style(ui_style_menutext);

		addstr(input);
		move(y,x+cursor_pos);

		move (y, x+cursor_pos);
		
		ui_style(ui_style_selected);
		addch (input[cursor_pos]);
		move (LINES - 1, COLS - 1);

		c = parsekey (ui_input (), ui_scope_lineedit);
		switch (c->action) {
			case ui_action_right:
				if (cursor_pos < (strlen (input) - 1))
					cursor_pos++;
				break;
			case ui_action_left:
				if (cursor_pos)
					cursor_pos--;
				break;
			case ui_action_bol:
				cursor_pos = 0;
				break;
			case ui_action_eol:
				cursor_pos = strlen (input) - 1;
				break;
			case ui_action_complete:
				if(cursor_pos==strlen(input)-1){
					char *tmpcommand;
					input[cursor_pos]=0;
					tmpcommand=cli_complete(input);
					strcpy(input,tmpcommand);
					cursor_pos=strlen(tmpcommand);
					input[cursor_pos] = ' ';
					input[cursor_pos + 1] = 0;
					input[cursor_pos + 2] = 0;
					status_draw();
					refresh();
				}
				break;
			case ui_action_cancel:
				strcpy (&input[0], data_backup);
				stop = 1;
				break;
			case ui_action_confirm:
				stop = 1;
				break;
			case ui_action_delete:
				if (cursor_pos < (strlen (input) - 1)) {
					cursor_pos++;
					if (cursor_pos) {
						memmove (&input[cursor_pos - 1],
								 &input[cursor_pos],
								 strlen (input) - cursor_pos);
						input[strlen (input) - 1] = 0;
						cursor_pos--;
					}
				}
				break;
			case ui_action_backspace:
				if (cursor_pos) {
					memmove (&input[cursor_pos - 1],
							 &input[cursor_pos], strlen (input) - cursor_pos);
					input[strlen (input) - 1] = 0;
					cursor_pos--;
				}
				break;

			default:
				if (c->action > 31 && c->action < 255) {	/*  input for buffer */
					memmove (&input[cursor_pos + 1],
							 &input[cursor_pos],
							 strlen (input) - cursor_pos + 1);
					input[cursor_pos++] = c->action;
				} 
				break;
		}
	}

	input[strlen (input) - 1] = 0;

	free(data_backup);

	return (input[0]);
}


void ui_getstr(char *prompt, char *datastorage){
	move(LINES-2,0);
	ui_style(ui_style_menuitem);
	addstr(prompt);
	clrtoeol();

	move(LINES-1,0);
	ui_style(ui_style_menuitem);
	addstr(">");

	ui_getstr_loc(datastorage, -1, -1, 80);
}
