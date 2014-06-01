/*
 * hnb.c -- the main app, of hierarchical notebook, an personal database
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


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tree.h"
#include "ui.h"
#include "file.h"
#include "path.h"
#include "prefs.h"
#include "cli.h"

static void usage (const char *av0)
{
	fprintf (stderr, "\nusage: %s [database] [options] [command [command] ..]\n", av0);
	fprintf (stderr, "\n\
Hierarchical NoteBook by Øyvind Kolås <pippin@users.sourceforge.net>\n\
It is distributed under the GNU General Public License\n\
\n\
default database: '%s'\n",prefs.default_db_file);
	fprintf(stderr,"\n\
Options:\n\
\n\
\t-h --help     this message\n\
\t-v --version  prints the version\n\
\t-t --tutorial loads the tutorial instead of a database\n\
\n\
\t-a --ascii    load ascii ascii\n\
\t   --hnb      load hnb DTD\n\
\t-x --xml      load general xml\n");
#ifdef USE_LIBXML
fprintf(stderr,"\t-s --stylized load stylized xml (using libxml2)\n");
#endif
fprintf(stderr,"\n\
\t-rc <file>        specify other config file\n\
\t-ui <interface>   interface to use, ( curses(default) or cli)\n\
\t-e                execute commands\n\
\n\n");
}

#define BUFFERLENGTH 4096
/* is currently set to 4096,.. should be removed and replaced by rigid code*/

char input[BUFFERLENGTH];
Node *pos;

#define undefined_key(a,c)\
	{if(c!=UI_IGNORE){char msg[80];\
	sprintf(msg," No action assigned to '%s'(%id) in %s-mode",keyname(c),c,a);\
	ui_draw(pos, msg, UI_MODE_ERROR);\
	}}\

#define info(a)\
	{ui_draw(pos, a, UI_MODE_INFO);\
	}\

#define infof(a,b)\
	{char msg[80];\
	sprintf(msg,a,b);\
	ui_draw(pos, msg, UI_MODE_INFO);\
	}\

#ifdef WIN32
	#undef undefined_key
	/*pdcurses in windows defines control keys etc.. flashing errors is not nice*/
	#define undefined_key(a,c)
#endif

int app_edit (int restricted){
	int c;
	int stop = 0;
	static int cursor_pos;
	static char *data_backup;
	static char input[BUFFERLENGTH];
	
	memset(input,0,sizeof(input));
	data_backup = pos->data;
	input[0] = 0;
	strcpy (&input[0], data_backup);
	pos->data = &input[0];
	cursor_pos = strlen (input);
	input[cursor_pos] = ' ';
	input[cursor_pos + 1] = 0;
	input[cursor_pos + 2] = 0;
	c = 0;
	
	while (!stop) {
		ui_draw (pos, (char *) cursor_pos, restricted?UI_MODE_EDITR:UI_MODE_EDIT);
		c = ui_input ();
		switch (c) {
			case UI_RIGHT:
				if (cursor_pos < (strlen (input) - 1))
					cursor_pos++;
				break;
			case UI_LEFT:
				if (cursor_pos)
					cursor_pos--;
				break;
			case UI_TOP:
			case UI_PUP:
				cursor_pos=0;
				break;
			case UI_BOTTOM:
			case UI_PDN:
				cursor_pos=strlen(input)-1;
				break;
			case UI_QUIT:
			case UI_ESCAPE:
				strcpy (&input[0], data_backup);
				pos->data = &input[0];
				cursor_pos = strlen (input);
				input[cursor_pos] = ' ';
				input[cursor_pos + 1] = 0;
				stop = 1;
				break;
			case UI_UP:
				if(hnb_edit_posup>=0)
				cursor_pos=hnb_edit_posup;
				break;
			case UI_DOWN:
				if(hnb_edit_posdown<strlen(input))
					cursor_pos=hnb_edit_posdown;
				else
					cursor_pos=strlen(input)-1;
				break;
			case UI_ENTER:
				stop = 1;
				break;
			case UI_REMOVE:
				if (cursor_pos < (strlen (input) - 1)){
					cursor_pos++;
					if (cursor_pos) {
						memmove (&input[cursor_pos-1], 
							&input[cursor_pos],
							 strlen (input) - cursor_pos);
						input[strlen(input)-1]=0;
						cursor_pos--;
					}
				}
				break;
			case UI_BACKSPACE:
			case UI_BACKSPACE2:
			case UI_BACKSPACE3:
				if (cursor_pos) {
					memmove (&input[cursor_pos-1], 
						&input[cursor_pos],
						 strlen (input) - cursor_pos);
					input[strlen(input)-1]=0;
					cursor_pos--;
				}
				break;
				/*ignored keypresses.. */
			case UI_INSERT:
				break;
			case UI_SPLIT:
				if(!restricted){input[strlen (input) - 1] = 0;
				node_insert_down(pos);
				if(input[cursor_pos]==' ')
					node_setdata(node_down(pos),&input[cursor_pos+1]);
				else
					node_setdata(node_down(pos),&input[cursor_pos]);
				input[cursor_pos]=' ';
				input[cursor_pos+1]=0;		
				}
				break;
			case UI_JOIN:
				if(!restricted)if(node_down(pos)){
					cursor_pos=strlen(input);
					strcpy(&input[cursor_pos-1],
						node_getdata(node_down(pos)));
					input[strlen(input)]=' ';
					input[strlen(input)+1]=0;
					if(node_right(node_down(pos))){
						ui_draw(pos, "won't remove it has children", UI_MODE_ERROR);
					} else {
						node_remove(node_down(pos));
					}
					cursor_pos--;
				}
				break;
			default:
				if (c > 31 && c < 255) {	/*  input for buffer */
					memmove (&input[cursor_pos + 1], 
						&input[cursor_pos],
						 strlen (input) - cursor_pos + 1);
					input[cursor_pos++] = c;
				} else if(!restricted){
					if(prefs.usertag && c < 28 && cursor_pos==0&&input[cursor_pos+1]==0){
						strcpy(&input[0],prefs.usertags[c]);
						cursor_pos=strlen(input);				
						input[cursor_pos++]=' ';
						input[cursor_pos++]=' ';
						input[cursor_pos--]=0;
					} else undefined_key ("edit", c);
				}
				break;
		}
	}

	input[strlen (input) - 1] = 0;

	pos->data = data_backup;
	node_setdata (pos, input);

	return(input[0]);

}

/* FIXME code duplication warning, this function and it's helping variables is 
  also defined in file.c */

Node *npos;
int tstartlevel;

static void	himport_node (int level, int flags, unsigned char priority, char *data){
	level = level + tstartlevel;
	
	while (nodes_left (npos) > level)
		npos = node_left (npos);
	if (nodes_left (npos) == level)
		npos = node_insert_down (npos);
	if (nodes_left (npos) < level)
		npos = node_insert_right (npos);
	node_setflags (npos, flags);
	node_setpriority (npos, priority);
	node_setdata (npos, data);
}

void node_duplicate_tree (Node *source, Node *target){
	int level, flags, priority, sstartlevel;
	char *data;
	
	tstartlevel = nodes_left (target);
	
	node_setflags (target, node_getflags (source));
	node_setpriority(target, node_getpriority(source));
	node_setdata (target, node_getdata (source));
	npos = target;
	
	if (node_right (source)) {
		source = node_right (source);
		sstartlevel = nodes_left (source);
		while ((source != 0) & (nodes_left (source) >= sstartlevel)) {
			level = nodes_left (source) - sstartlevel + 1;
			flags = node_getflags (source);
			priority = node_getpriority(source);
			data = node_getdata (source);
			himport_node (level, flags,priority, data);
			source = node_recurse (source);
		}
	}
}

void app_mark (){
	{
		int stop = 0;
		Node *marked = pos;
		
		pos = node_insert_down (pos);
		node_setdata (pos, "-=- destination -=-");
		input[0] = 0;
		while (!stop) {
			int c;
			
			ui_draw (pos, node_getdata(marked), UI_MODE_MARKED);
			c = ui_input ();
			switch (c) {
				case UI_DEBUG:
					ui_draw (pos, input, UI_MODE_DEBUG);
					getch ();
					break;
				case UI_UP:
					if (node_up (pos)) {
						node_swap (pos, node_up (pos));
						pos = node_up (pos);
						if (pos == marked)
							marked = node_down (pos);
					}
					break;
				case UI_DOWN:
					if (node_down (pos)) {
						node_swap (pos, node_down (pos));
						pos = node_down (pos);
						if (pos == marked)
							marked = node_up (pos);
					}
					break;
				case UI_LEFT:
					if (node_left (pos)) {
						Node *tnode = node_insert_down (node_left (pos));
						node_swap (tnode, pos);
						node_remove (pos);
						pos = tnode;
					}
					break;
				case UI_RIGHT:
					if (node_up (pos) && (node_up (pos) != marked)) {
						if (node_right (node_up (pos))) {	
							Node *tnode =
								node_insert_up (
									node_right (
										node_up (pos)));
							node_swap (tnode, pos);
							node_remove (pos);
							pos = tnode;
						} else {	/* if there are no children */
							Node *tnode = node_insert_right (
								node_up (pos));
							
							node_swap (tnode, pos);
							node_remove (pos);
							pos = tnode;
						}
					}
					break;
				case ' ':
					node_swap (marked, pos);
					node_remove (marked);
					stop = 1;
					break;
					
				/* clone?????,.. this is the place to add symlinks,.. 
					but how to implement it in the structure? */
				
				case UI_ENTER:
				case 'c':
				case 'C':		/*copy */
					node_duplicate_tree (marked, pos);
					stop = 1;
					break;
				case UI_ESCAPE:
				case 'q':
				case 'Q':
					stop = 1;
					node_remove (pos);
					pos = marked;
					break;
				default:
					undefined_key ("mark", c);
					break;
			}
		}
		stop = 0;
	}
}

int app_quit (){	/* returns 1 when user says quit */
	int c;
	
	ui_draw (pos, input, UI_MODE_QUIT);
	c = ui_input ();
	switch (c) {
		case 'y':
		case 'Y':
		case 'x':
		case 'X':
		case UI_QUIT:
			if (prefs.db_file[0] != (char) 255) {
				ptr_export ((Node *) node_root (pos), 
					prefs.db_file);
				ui_draw (pos, input, UI_MODE_HELP0 + prefs.help_level);
				infof (" wrote stuff to '%s'", prefs.db_file);
			}
			return (1);
			break;
		case 'q':
		case 'Q':
			info (" quitting without saving.");
			return (1);
		case 's':
		case 'S':
			if (prefs.db_file[0] != (char) 255) { /* fixme eehh? */
				ptr_export ((Node *) node_root (pos), 
					prefs.db_file);
				ui_draw (pos, input, UI_MODE_HELP0 + prefs.help_level);
				infof (" wrote stuff to '%s'", prefs.db_file);
			}
			return (0);
			break;
		case 'C':
		case 'c':
		case UI_ESCAPE:
			return (0);
		default:
			undefined_key ("quit", c);
			return (0);
	}
}

void app_search (){
	char query[100] = "Find: ";
	Node *query_start = pos;
	int query_start_level = nodes_left (pos);
	
	/* query user for search term */
	ui_draw (pos, (char *) query, UI_MODE_GETSTR);	
	
	pos = node_recursive_match ((char *) query, pos);
	
	if (pos == 0) {
		pos = query_start;
		ui_draw (pos, input, UI_MODE_HELP0 + prefs.help_level);
		infof (" search for '%s' returned emptyhanded", query);
		return;
	}
	
	while (pos != 0 && (nodes_left (pos) >= query_start_level)) {
		int c;
		
		ui_draw (pos, (char *) query, UI_MODE_SEARCH);
		c = ui_input ();
		switch (c) {
			case 's':
			case 'S':
			case UI_ENTER:
			{
				return;
			}
				break;
			case 'c':
			case 'C':
			case 'q':
			case 'Q':
			case UI_ESCAPE:
			{
				pos = query_start;
				return;
			}
				break;
			case 'n':
			case 'N':
			case ' ':
				pos = node_recursive_match ((char *) query, pos);
				break;
			default:
				undefined_key ("search", c);
				break;
		}
	}
	pos = query_start;
	ui_draw (pos, input, UI_MODE_HELP0 + prefs.help_level);
	info (" end of search");
}

void app_remove (){
	if (node_right (pos)) {
		int c;
		
		ui_draw (pos, "node has children, really remove?", UI_MODE_CONFIRM);
		c = ui_input ();
		if ((c == 'y') || (c == 'Y'))
			pos = node_remove (pos);
	} else {
		pos = node_remove (pos);
	}
}

void app_export (){
	int c, stop = 0;
	char filename[100];
	
	while (!stop) {
		ui_draw (pos, "", UI_MODE_EXPORT);
		c = ui_input ();
		switch (c) {
			case 'x':case 'X':
				strcpy ((char *) filename, "File to save hnb xml output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					hnb_export (node_top (pos), filename);
				stop = 1;
				break;
			case '?':
				strcpy ((char*) filename, "Save help-include file in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					help_export (node_top (pos), filename);
				stop = 1;
				break;
			case 'h':
			case 'H':
				strcpy ((char *) filename, "File to save html output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					html_export (node_top (pos), filename);
				stop = 1;
				break;
			case 'a':
			case 'A':
				strcpy ((char *) filename, "File to save ascii output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					ascii_export (node_top (pos), filename);
				stop = 1;
				break;
			case 'g':
			case 'G':
				strcpy ((char *) filename, "File to save general xml output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					xml_export (node_top (pos), filename);
				stop = 1;
				break;
#ifdef USE_XML
			case 's':
			case 'S':
				strcpy ((char *) filename, "File to save stylized xml output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					libxml_export (node_top (pos), filename);
				stop = 1;
				break;
#endif
			case '|':
				strcpy ((char *) filename, "command line (%s for file):");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (!strlen (filename))
					return;
				ui_end();
				ascii_export(node_top(pos), "hnb.tmp.2.txt");
				{
					char cmd_buf[400];
					sprintf(cmd_buf,filename,"hnb.tmp.2.txt");
					system(cmd_buf);
				}
				ui_init();
				stop=1;
				break;
			case 'c':
			case 'C':
			case UI_ESCAPE:
				stop = 1;
				break;
			default:
				undefined_key ("export", c);
		}
	}
}

void app_import (){
	int c, stop = 0;
	char filename[100];
	
	while (!stop) {
		ui_draw (pos, "", UI_MODE_IMPORT);
		c = ui_input ();
		switch (c) {
			case 'x':case 'X':
				strcpy ((char *) filename, "xml file to import:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					pos=hnb_import (node_top (pos), filename);
				stop = 1;
				break;
			case 'a':
			case 'A':
				strcpy ((char *) filename, "ascii file to import:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					pos=ascii_import (node_top (pos), filename);
				stop = 1;
				break;
			case 'g':
			case 'G':
				strcpy ((char *) filename, "general xml file to import:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					pos=xml_import (node_top (pos), filename);
				stop = 1;
				break;
#ifdef USE_LIBXML
			case 's':
			case 'S':
				strcpy ((char *) filename, "stylized xml file to import:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					pos=libxml_import (node_top (pos), filename);
				stop = 1;
				break;
#endif
			case 'c':
			case 'C':
			case UI_ESCAPE:
				stop = 1;
				break;
			default:
				undefined_key ("import", c);
		}
	}
}

void app_prefs (){
	Node *tpos=pos; /*store orignal pos in original tree*/
	int stop = 0;
	int percent=prefs.showpercent;
	pos=load_prefs();
	prefs.showpercent=0;
	
	while (!stop) {
		int c;
		
		ui_draw (pos, input, UI_MODE_PREFS);
		c = ui_input ();
		switch (c) {
			case UI_QUIT:case UI_ESCAPE:case UI_PREFS:case 'x':case 'c':case 'X':
				stop = 1;
				break;
			case UI_SAVE:case 's':case 'S':
					save_prefs(pos);
					tree_free(pos);
					pos=tpos;
					ui_draw (pos, input, UI_MODE_HELP0 + prefs.help_level);
					infof (" saved config in %s", prefs.rc_file);
					prefs.showpercent=percent;
					return;
				break;
			case 1:	case 'a':case 'A':
					apply_prefs(pos);
					percent=prefs.showpercent;
					prefs.showpercent=0;
				break;
			case UI_UP:
				if (node_up (pos))
					pos = node_up (pos);
				break;
			case UI_DOWN:
				if (node_down (pos))
					pos = node_down (pos);
				break;
			case UI_LEFT:
				if (node_left (pos))
					pos = node_left (pos);
				break;
			case UI_RIGHT:
				if (node_right (pos)){
					pos = node_right (pos);
					break;
				}
			case UI_ENTER:case ' ':case UI_TOGGLE_DONE:
				switch node_getpriority(pos){
					case 1:node_toggleflag(pos,F_done); /* checkbox */
						break;
					case 2:{ /* radio button */
						Node *tnode=node_top(pos);
						while(tnode){
							if(node_getpriority(tnode)==2)
								node_setflag(tnode,F_done,0);
							tnode=node_down(tnode);
						}
						node_setflag(pos,F_done,1);
						break;
					}
					case 3: /* editable item */
						app_edit(1);
						break;
					case 4: /* color */
						break;
					case 5: /* shortcut */
					
						break;
					default:
						if(node_getpriority(node_right(pos))==3){
							pos=node_right(pos);
							app_edit(1);
						}
						break;
				}
				break;
		}
	}
	tree_free(pos);
	pos=tpos;
	prefs.showpercent=percent;	
}

int hnb_nodes_down;
int hnb_nodes_up;

void app_navigate (){

#define chktemp		if (node_getflag(pos,F_temp)){\
					pos = node_remove (pos);\
					node_update_parents_todo (pos);\
				}


	int stop = 0;
	
	while (!stop) {
		int c;
		
		ui_draw (pos, input, UI_MODE_HELP0 + prefs.help_level);
		c = ui_input ();
		switch (c) {
			case UI_EXPORT:
				app_export ();
				break;
			case UI_QUIT:
				stop = app_quit ();
				break;
			case UI_FIND:
				app_search ();
				break;
			case UI_MARK:
				app_mark ();
				break;
			case UI_IMPORT:
				app_import();
				break;
			case UI_SAVE:
				if (prefs.db_file[0] != (char) 255) {
					ptr_export ((Node *) node_root (pos), 
						prefs.db_file);
					ui_draw (pos, input, UI_MODE_HELP0 + prefs.help_level);
					infof (" wrote stuff to '%s'", prefs.db_file);
				}
				break;
			case UI_HELP:case 26:
				prefs.help_level++;
				if (prefs.help_level >= 3)
					prefs.help_level = 0;
				break;
			case UI_TOGGLE_TODO:
				node_toggleflag(pos,F_todo);
				node_update_parents_todo(pos);
				break;
			case UI_TOGGLE_DONE:
				node_toggleflag(pos,F_done);
				node_update_parents_todo(pos);
				break;
			case UI_SORT:
				pos = node_sort_siblings (pos);
				break;
			case UI_LOWER:
				pos = node_lower(pos);
				node_update_parents_todo (pos);				
				break;
			case UI_RAISE:
				pos = node_raise(pos);
				break;
			case UI_TOP:
				chktemp;
				input[0] = 0;
				pos = node_root (pos);
				break;
			case UI_UP:
				chktemp else {
					if (node_up (pos))
						pos = node_up (pos);
					else if(prefs.forced_up){
						if(node_left(pos))
							pos=node_left(pos);
					}
				}
				input[0] = 0;			
				break;
			case UI_DOWN:
				chktemp else if (node_down (pos))
					pos = node_down (pos);
					else if(prefs.forced_down){
						while(node_left(pos)){
							if(node_down(pos)){
								break;
							}
							pos=node_left(pos);
						}
					if(node_down(pos))
						pos=node_down(pos);
					}
				input[0] = 0;
				break;
			case UI_PDN:
				input[0] = 0;
				{
					int n;
					for (n = 0; n < hnb_nodes_down-1; n++)
						if (node_down (pos))
							pos = node_down (pos);
				}
				break;
			case UI_PUP:
				chktemp;
				input[0] = 0;
				{
					int n;
					for (n = 0; n < hnb_nodes_up; n++)
						if (node_up (pos))
							pos = node_up (pos);
				}
				break;
			case UI_LEFT:
				chktemp else {
					if (node_left (pos))
						pos = node_left (pos);
				}
				input[0] = 0;
				break;
			case UI_RIGHT:
				if (node_right (pos)) {
					pos = node_right (pos);
				} else {				
					if(node_getdata(pos)[0]){
						node_insert_right(pos);
						if (node_getflag(pos,F_temp))	
							node_setflag(pos,F_temp,0);
						if(node_getflag(pos,F_todo))
							node_setflag(node_right(pos),F_todo,1);	
						node_setflag(node_right(pos),F_temp,1);
						pos=node_right(pos);
					}
				}
				input[0] = 0;
				break;
			case UI_REMOVE:
				app_remove ();
				node_update_parents_todo (pos);
				break;
			case UI_COMPLETE:
				if (strcmp (input, node_getdata (pos)) == 0) {
					if (node_right (pos)) {
						pos = node_right (pos);
					} else {
						if(node_getdata(pos)[0]){
							node_insert_right(pos);
							if (node_getflag(pos,F_temp))
								node_setflag(pos,F_temp,0);
							if(node_getflag(pos,F_todo))
								node_setflag(node_right(pos),F_todo,1);	
							node_setflag(node_right(pos),F_temp,1);
							
							pos=node_right(pos);
						}
					}
					input[0] = 0;
				} else {
					strcpy (input, node_getdata (pos));
				}
				break;
			case UI_ESCAPE:
				if (node_getflag (pos, F_temp)) {
					pos = node_remove (pos);
				} else {
					stop = app_quit ();
				}
				input[0] = 0;
				break;
			case UI_ENTER:
				if (!strlen (input)) {
					app_edit (0);
				} else {
					if (node_getflag(pos,F_temp))	{
						node_setflag (pos, F_temp,0);
						node_update_parents_todo (pos);
					} else {
						pos = node_insert_down (node_bottom (pos));
						node_setdata (pos, input);
						if (node_getflag (node_left (pos), F_todo)) {
							node_setflag (pos, F_todo,1);
							node_update_parents_todo (pos);
						}
					}
					input[0] = 0;
				}
				break;
			case UI_PREFS:
				app_prefs();
				break;
			case UI_PRIORITY: /* ^P priority */
				if (node_getflag(pos,F_todo))	{
					if(node_getpriority(pos))
						sprintf(input,"Current priority [%i], enter new (1-5) press return to keep",node_getpriority(pos));
					else 
						sprintf(input,"Current priority [--], enter new (1-5) press return to keep");					
					ui_draw (pos, input, UI_MODE_GETSTR);
					if (strlen (input)){
						int p=atoi(input);
						if(p>=0 && p<6)
							node_setpriority(pos,p);
					}
					input[0]=0;
				}
				break;
			case UI_BACKSPACE:
			case UI_BACKSPACE2:
			case UI_BACKSPACE3:
				if (!strlen (input)) {
					app_remove ();
				} else {
					input[strlen (input) - 1] = 0;
					if (node_getflag(pos,F_temp))	
						if (node_up (pos))
							pos = node_remove (pos);
				}
				break;
			case UI_INSERT:
				if(node_getflag(pos,F_temp)){
					node_toggleflag(pos,F_temp);
				} else {
					pos = node_insert_down (pos);
					if (node_left (pos))
						if (node_getflag(node_left(pos),F_todo))
							node_setflag (pos, F_todo,1);
					node_update_parents_todo (pos);
				}
				
				if(!app_edit (0))
					pos=node_remove(pos);
				input[0]=0;
				break;
			
			default:
				if (c > 31 && c < 255) {	/*  input for buffer */
					input[strlen (input) + 1] = 0;
					input[strlen (input)] = c;
				} else
					undefined_key ("navigation", c);
				break;
		}
		
		if (strlen (input)) {
			if (node_getflag(pos,F_temp))	 {
				node_setdata (pos, input);
			} else {
				if (node_match (input, pos)) {
					if (strcmp
						(node_getdata (node_match (input, pos)),
						 node_getdata (pos)) != 0)
						pos = node_match (input, pos);
				} else {
					pos = node_insert_down (node_bottom (pos));
					node_setflag (pos, F_temp,1);
					node_setdata (pos, input);
					if (node_left (pos))
						if (node_getflag(node_left(pos),F_todo))
							node_setflag (pos, F_todo,1);
				}
			}
		}
	}
}

int main(int argc,char **argv){
	int argno;
		/* current commandline argument in focus */
	
	struct{		/* initilaized defaults */
		int version;
		int usage;
		int def_db;
		int format;
		int ui;
		int tutorial;
		int debug;
		char *dbfile;
		char *rcfile;
		char *cmd; 
	}cmdline={
		0,   /* version */
		0,   /* usage */
		1,   /* load default db*/
		-1, /*format to load by default*/
		1,	 /* ui */
		0,	 /* tutorial */
		0,	/*debug*/
		0,0,0
	};
	
	{/*parse commandline*/
		for (argno = 1; argno < argc; argno++) {
			if ( !strcmp(argv[argno], "-h") 
					|| !strcmp (argv[argno], "--help")) {
				cmdline.usage=1;
			} else if (!strcmp(argv[argno], "-v") 
					|| !strcmp (argv[argno], "--version")) {
				cmdline.version=1;			
			} else if (!strcmp(argv[argno], "-t") 
					|| !strcmp (argv[argno], "--tutorial")) {
				cmdline.tutorial=1;
			} else if (!strcmp(argv[argno], "-a") 
					|| !strcmp (argv[argno], "--ascii")) {
				cmdline.format=FORMAT_ASCII;
			} else if (!strcmp(argv[argno], "-hnb") 
					|| !strcmp (argv[argno], "--hnb")) {
				cmdline.format=FORMAT_HNB;
			} else if (!strcmp(argv[argno], "-x") || !strcmp(argv[argno], "-gx") 
					|| !strcmp (argv[argno], "--xml")) {
				cmdline.format=FORMAT_XML;
#ifdef USE_LIBXML
			} else if (!strcmp(argv[argno], "-s") || !strcmp(argv[argno], "-sx") 
					|| !strcmp (argv[argno], "--stylized")) {
				cmdline.format=FORMAT_LIBXML;
#endif
			} else if (!strcmp(argv[argno], "-ui") ) {
				if(!strcmp(argv[++argno],"curses")){
					cmdline.ui=1;
				} else if(!strcmp(argv[argno],"cli")){
					cmdline.ui=2;
				} else if(!strcmp(argv[argno],"gtk") 
					|| !strcmp(argv[argno],"gtk+") ){
					cmdline.ui=3;
				} else {
					fprintf (stderr, "unknown interface %s\n", 
						argv[argno]);
					exit(1);				
				}				
			} else if (!strcmp(argv[argno], "-rc") ) {
				cmdline.rcfile=argv[++argno];
			} else if (!strcmp(argv[argno], "-e") ) {  
			/* actually just a dummy option to specify default db */
				if(!cmdline.dbfile){
					cmdline.def_db=1;      
					cmdline.dbfile=(char*)-1;
				}
			} else if(!strcmp(argv[argno],"-d")){
				cmdline.debug=1;
			} else {
				if(argv[argno][0]=='-'){
					fprintf (stderr, "unknown option %s\n", 
						argv[argno]);	
					exit(1);
				} else if(!cmdline.dbfile){
					cmdline.dbfile=argv[argno];
					cmdline.def_db=0;
				} else {
					cmdline.cmd=argv[argno];
					cmdline.ui=0;
					argno++;
					break; /* stop processing cmd args */
				}
			}
		}
	}
	
	init_prefs();
	
	if(cmdline.usage){
		usage (argv[0]);
			exit (0);		
	}
	
	if(cmdline.version){
		fprintf (stderr,"%s %s\n",PACKAGE, VERSION);
			exit (0);
	}
	
	if(cmdline.rcfile){
		strcpy(prefs.rc_file,cmdline.rcfile);
	}

	{/*rc file sanity*/
		if(!file_check(prefs.rc_file)){
			write_def_rc();
			fprintf(stderr,"created %s for configuration\n",prefs.rc_file);
			sleep(1);
		} else if(!xml_check(prefs.rc_file)){
			fprintf(stderr,"%s does not seem to be a xml file, and thus cannot be a valid config file for hnb, please remove it.\n",prefs.rc_file);
			exit(1);
		}
	}

	pos=load_prefs();
	apply_prefs(pos);
	tree_free(pos);
	
	/* ovveride the prefs with commandline specified options*/
		if(cmdline.debug)
			prefs.debug=1;
		if(cmdline.tutorial)
			prefs.tutorial=1;
		if(cmdline.format!=-1){ /* format specified */
			prefs.format=cmdline.format;
		} else {
			prefs.format=FORMAT_HNB;/*prefs.def_format;*/
		}
			
	if(cmdline.def_db){
		strcpy(prefs.db_file,prefs.default_db_file);
		if(!file_check(prefs.db_file))
			prefs.tutorial=1;
	} else {
		strcpy(prefs.db_file,cmdline.dbfile);
	}

	switch(prefs.format){
		case FORMAT_ASCII:
			ptr_import = ascii_import;
			ptr_export = ascii_export;
			break;
		case FORMAT_HNB:
			ptr_import = hnb_import;
			ptr_export = hnb_export;
			break;
		case FORMAT_XML:
			ptr_import = xml_import;			
			ptr_export = xml_export;
			break;
#ifdef USE_LIBXML
		case FORMAT_LIBXML:
			ptr_import = libxml_import;			
			ptr_export = libxml_export;
			break;
#endif
	}
	
	input[0] = 0;
	pos = tree_new ();
	
	if(!prefs.tutorial){
		if(FORMAT_HNB == prefs.format)
		{
			if(!xml_check(prefs.db_file)){
				fprintf(stderr,"%s does not seem to be a xml file, aborting.\n\
if this is an old ascii hnb file, you can convert it with:\n\
\thnb -a \"%s\" \"export -s %s\"\n", prefs.db_file, prefs.db_file, prefs.db_file);
				exit(1);
			}
		}
		pos = ptr_import (pos, prefs.db_file);
	}
	
	if(prefs.tutorial){
		prefs.db_file[0] = (char) 255; 	/* disable saving */
		pos = help_import (pos);
	}
	
	switch(cmdline.ui){
		case 1:
			ui_init ();
			app_navigate ();
			ui_end ();
			break;
		case 0:
			pos=docmd(pos,cmdline.cmd);
			while(argno<argc){
				pos=docmd(pos,argv[argno++]);
			}
			break;
		case 2:
			pos=cli(pos);
			break;
		case 3:
			printf("gtk+ interface not implemented\n");
			break;
	}
	
	tree_free(pos);

	if(cmdline.debug)
		prefs.debug=0;
	
	if(prefs.debug){
		fprintf(stderr,"show help: \t%i\n",cmdline.usage);
		fprintf(stderr,"show version:\t%i\n",cmdline.version);
		fprintf(stderr,"ui:     \t%i\n",cmdline.ui);
		fprintf(stderr,"tutorial:\t%i\n",cmdline.tutorial);
		fprintf(stderr,"default db:\t\"%s\"\n",prefs.default_db_file);
		fprintf(stderr,"format: \t%i\n",cmdline.format);
		fprintf(stderr,"dbfile: \t\"%s\"\n",prefs.db_file);
		fprintf(stderr,"rcfile: \t\"%s\"\n",prefs.rc_file);
		fprintf(stderr,"cmd1:   \t\"%s\"\n",cmdline.cmd);
	}
	
	if(prefs.rc_rev<RC_REV){
fprintf(stderr,"your preferences file (%s) seems to be from an older version\n\
of hnb to gain the ability to set the new preferences please remove it and\n\
let hnb install a new default one.\n\nNew features:",prefs.rc_file);
	switch (prefs.rc_rev){
		case 0:fprintf(stderr," toggle mouse support,");
		case 1:fprintf(stderr," priority colors");
	}
	fprintf(stderr,"\n");
	}
	
	return 0;
}
