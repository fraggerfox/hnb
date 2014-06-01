/*
 * ui_draw.c -- drawing of a nodes and trees using curses
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
#include <assert.h>
#include "tree.h"
#include "tree_todo.h"
#include <string.h>
#include <unistd.h>
#include "curses.h"
#include "stdio.h"
#include "prefs.h"
#include "ui_overlay.h"
#define UI_C
#include "ui.h"
#include "ui_draw.h"
#include "ui_style.h"
#include "cli.h"
#include <stdlib.h>

int nodes_above;
int active_line;
int nodes_below;

static Node *up (Node *sel, Node *node)
{
	switch (prefs.collapse_mode) {
		case COLLAPSE_ALL:
		case COLLAPSE_ALL_BUT_CHILD:
			if (node_up (node))
				return (node_up (node));
			else
				return (node_left (node));
			break;
		case COLLAPSE_NONE:
			return node_backrecurse (node);
			break;
		case COLLAPSE_ONLY_SIBLINGS:
			return node_up (node);
			break;
		case COLLAPSE_PATH:
			if (nodes_left (node) == nodes_left (sel))
				if (node_up (node))
					return (node_up (node));
			return (node_left (node));
			break;
	}
	return 0;
}

static Node *down (Node *sel, Node *node)
{
	switch (prefs.collapse_mode) {
		case COLLAPSE_ALL_BUT_CHILD:
			if ((node == sel) && (node_right (node)))
				return node_right (node);
		case COLLAPSE_ALL:
			if (node_down (node)) {
				return (node_down (node));
			} else {
				while (node != 0) {
					node = node_left (node);
					if (node_down (node))
						return (node_down (node));
				}
			}
			break;
		case COLLAPSE_NONE:
			return node_recurse (node);
			break;
		case COLLAPSE_ONLY_SIBLINGS:
			return node_down (node);
			break;
		case COLLAPSE_PATH:
			return node_down (node);
			break;
	}
	return (0);
}

int startlevel = 0;

int hnb_edit_posup = 0;			/*contains the cursor pos for up/down */
int hnb_edit_posdown = 0;		/*from here when in editing mode */

enum {
	drawmode_test=0,
	drawmode_normal,
	drawmode_selected,
	drawmode_edit,
	drawmode_completion
};

/* draws checkbox, percentage completed, size etc.
 * @param line the line to draw it on
 * @param node the node to extract information from
 * @param drawmode onle checked wether it is test or not.
 * @returns number of characters used/needed
 */
/*static int
	prefs_showpercent=1,
    prefs_showbox=1,
	prefs_bulletmode=BULLET_PLUSMINUS,
	prefs_showsize=1,
	prefs_showaccsize=1,
	prefs_showpercentage=1,
	prefs_showdonesize;
*/
/*
	prematter order?
	syntax parser?
	
	
	"+" "P%"|"[X]" S D/A
*/

/*
	NB: the function below contains more advanced visualisation
	possibilities than currently used in the application,
	this will hopefully be amended in the applications later
*/
static int draw_prematter(int line, int colstart, Node *node,int drawmode){
	int completion;
	int space_used=0;

	int asize;
	int size=node_getsize(node);
	int perc;

	perc=calc_percentage_size(node, &asize);
	{
		ui_style(ui_style_bullet);

		move(line,colstart+space_used);
			switch(perc){
				case -1:
					if(drawmode!=drawmode_test)
						switch(prefs.bulletmode){
							case BULLET_NONE:
								break;
							case BULLET_PLUSMINUS:
								addstr( (node_right(node)) ? "  +":"  -");
								break;
							case BULLET_STAR:
								addstr("  *");
								break;
							case BULLET_MINUS:
								addstr("  -");			
								break;
						}
					space_used+=3;
					break;
				case -2:
					space_used+=3;	
					break;
				case 0:
					if(drawmode!=drawmode_test)
						addstr ("[ ]");
					space_used+=3;
					break;
				case 2000:
					if(drawmode!=drawmode_test)
						addstr ("[X]");
					space_used+=3;
					break;
				default:{
					char str[10];
					sprintf (str, "%2i%%", perc);
					if(drawmode!=drawmode_test)
						addstr(str);
					space_used+=strlen(str);
					}
			}
	}

	{
		if(drawmode!=drawmode_test)
			addstr(" ");
		space_used+=1;
	}
/*	
	{
		char str[64];
		sprintf(str,"(%i/%i)",(  
		    perc==2000?100:perc
			
			
			*asize)/100 ,asize);
		if(drawmode!=drawmode_test)
			addstr(str);
		space_used+=strlen(str)+1;	
	}
*/
	return space_used;

	{
		perc=calc_percentage_size(node, &asize);
		attrset(A_NORMAL);
		move(line,colstart);
		{
			char str[256];
			sprintf (str, "size:%i a_size:%i %i%%", node_getsize(node),size,perc);
			addstr(str);
		}
		return 25;
	}
	
	if(drawmode!=drawmode_test){
		ui_style(ui_style_bullet);

		completion=done_status(node);

		if(completion==-2);else
		if(completion==-1){	
			move(line,colstart+2);
			switch(prefs.bulletmode){
				case BULLET_NONE:
					break;
				case BULLET_PLUSMINUS:
					addch( (node_right(node)) ? '+':'-');
					break;
				case BULLET_STAR:
					addch('*');
					break;
				case BULLET_MINUS:
					addch('-');			
					break;
			}
		} else {
			move(line,colstart);
			switch(completion){
				case 0:
					addstr ("[ ]");
					break;
				case 2000:
					addstr ("[X]");
					break;
				default:{
					char str[10];
					sprintf (str, "%2i%%", completion / 10);
					addstr(str);
					}
			}
		}
		if(node_calc_size(node)!=-1){
			char str[10];
			move(line,colstart+3);
			sprintf (str, "%4.1f", (float)node_calc_size(node)/10.0  );
			addstr(str);			
		}
		space_used+=4;
		space_used+=4;
	} else {
		space_used+=4;
		space_used+=4;
	} 
	
	return space_used;
		return 1;
}

/*
 * @param line_start which line on the display the first line of the draw node is on
 * @param level      the indentation level of this item
 * @param node       the node to draw
 * @param cursor_pos different meanings in different modes, testmode: none
 *                   highlightmode: none, edit_mode: the position in the data
 *                   that should be highlighted,
 *                   completion: the number of matched chars in data
 *                   
 * @param draw_mode  1=draw, 0=test
 *
 * @return number of lines needed to draw item
 **/
static int draw_item(int line_start, int cursor_pos, Node *node, int drawmode){
	int col_start      /* left margin of the actual text area, not the additional bullets and stuff */
		=(nodes_left(node)-startlevel)*prefs.indent;
	int col_end = COLS;
	
	unsigned char word[200];    /* current word being rendered */
	int wpos=0;        /* position in current word */
	int dpos=0;        /* position in data*/
	int col; 		   /* current column */

	int lines_used=1;

	int cursor_state=0;
	int cx=0,cy=0; /* coordinates to draw cursor at */

	unsigned char *data=(unsigned char*)node_getdata(node);

	col_start+=draw_prematter(line_start,col_start,node,drawmode);
	col=col_start;

	word[0]=0;
	if(drawmode==drawmode_edit){
		hnb_edit_posup =0;
		hnb_edit_posdown=strlen((char *)data);
	}

	switch(drawmode){
		case drawmode_test:
			break;
		case drawmode_completion:
			if(node_right(node)){
				ui_style(ui_style_parentnode);
			} else {
				ui_style(ui_style_node);
			}
			break;
		case drawmode_selected:
			if(node_right(node)){
				ui_style(ui_style_parentselected);
			} else {
				ui_style(ui_style_selected);
			}
			break;
		case drawmode_normal:
		case drawmode_edit:
		default:
			if(node_right(node)){
				ui_style(ui_style_parentnode);
			} else {
				ui_style(ui_style_node);
			}
			break;
	}


	while((dpos==0)||data[dpos-1]){ /* loop through data + \0 */
		switch(data[dpos]){
			case '\0':  /* \0 as well,.. to print last word */
			case ' ':
			case '\t':
			case '\n':
			case '\r': /* all whitespace is treated as spaces */
				if(col + wpos + 1 >= col_end){ /* reached margin */
					if(drawmode==drawmode_edit){
						if(cursor_state==0)
							hnb_edit_posup= cursor_pos-(col-col_start);
						if(cursor_state==1){
							hnb_edit_posdown= cursor_pos+(col-col_start);
							cursor_state=2;
						}
					}
				
					col = col_start;
					lines_used++;
					if(lines_used+line_start>=LINES)
						return lines_used;
				}
				if(drawmode!=drawmode_test){
					if(line_start+lines_used -1>=0){
						move(line_start+lines_used-1,col);
						
						/* must break the word in two due to differnt text formatting */
						if(drawmode==drawmode_completion && cursor_state==0 && dpos>=cursor_pos){ 
							int i;
							for(i=0;i< wpos-(dpos-cursor_pos)  ;i++)
								addch(word[i]);
								if(node_right(node)){
									ui_style(ui_style_parentselected);
								} else {
									ui_style(ui_style_selected);
								}
							for(i=wpos-(dpos-cursor_pos);i<wpos  ;i++)
								addch(word[i]);	
						} else {
						
							addstr((char *)word);
							
						}
						if(data[dpos])
							addch(' ');
					}
				}

				switch(drawmode){
					case drawmode_edit:
						if(cursor_state==0 && dpos>=cursor_pos) {
							cy = line_start + lines_used - 1;
							cx = col - (dpos - cursor_pos) + wpos;
							cursor_state=1;
						}
						break;
					case drawmode_completion:
						if(cursor_state==0 && dpos>=cursor_pos) {
							if(node_right(node)){
								ui_style(ui_style_parentselected);
							} else {
								ui_style(ui_style_selected);
							}
							cursor_state=1;
						}
					default:
						break;
				}


				col+=wpos+1;
				word[wpos=0]=0;
				break;
			default:
				if(wpos<198){
							word[wpos++]=data[dpos];
							word[wpos]=0;
				}
				break;
		}
		dpos++;
	}

	/* draw the cursor */
	if (drawmode==drawmode_edit) {
		move (cy, cx);
		if(node_right(node)){
			ui_style(ui_style_parentselected);
		} else {
			ui_style(ui_style_selected);
		}
		addch (data[cursor_pos]);
		if(node_right(node)){
			ui_style(ui_style_parentnode);
		} else {
			ui_style(ui_style_node);
		}
	}
	return lines_used;
}



extern int hnb_nodes_up;
extern int hnb_nodes_down;

void ui_draw (Node *node, Node *lastnode, char *input, int edit_mode)
{

/*	static Node *lastnode;*/
	int lines;
	static struct {
		int parent;
		int child;
		int up;
		int down;
	} lines2 = {
	1, 1, 1, 1};
	/*FIXME?: calculate .. startlevel,.. making the interface move right if the  nodes are deeply nested */
	/* do some magic to figure out which line is the active one */
	
	if (!prefs.fixedfocus) {
#define KEEPLINES 5

		int maxline =
			LINES - (prefs.help_level ? prefs.help_level ==
					 1 ? 6 : 1 : 2) - KEEPLINES;
		if (node_up (node) == lastnode) {
			active_line += lines2.down;
		} else if (node_down (node) == lastnode) {
			if (active_line > KEEPLINES)
				active_line -= lines2.up;
		} else if (node_left (node) == lastnode) {
			active_line += 1/*lines2.child is unneccesary,.. since it always will be */;
		} else if (node_right (node)
				   && node_right (node) == node_top (lastnode)) {
			active_line -= lines2.parent;
		}
		if (!lastnode)
			active_line--;
		if (active_line > maxline)	/*if we overlap with help,.. move up */
			active_line = maxline;
		if (active_line < KEEPLINES)
			active_line = KEEPLINES;
	}
	lines2.down = 0;
	lines2.up = 0;
	lines2.parent = 0;
	lines2.child = 0;

	nodes_above = active_line;
	nodes_below = LINES - active_line;

		{
			hnb_nodes_up = 0;
			hnb_nodes_down = 0;

			erase ();
/* draw nodes above selected node */
			{
				Node *prev_down = node;	/* to aid pgup/pgdn */
				int line = active_line;
				Node *tnode = up (node, node);

				while (tnode) {
					draw_item (line -= draw_item (0, 0, tnode, drawmode_test), 0, tnode, drawmode_normal);
					if (!lines2.up && node_down (tnode) == node)
						lines2.up = active_line - line;
					if (!lines2.parent && node_right (tnode)
						&& node_right (tnode) == node_top (node))
						lines2.parent = active_line - line;

					if (node_down (tnode) == prev_down) {
						hnb_nodes_up++;
						prev_down = tnode;
					}

					tnode = up (node, tnode);
					if (active_line - nodes_above >= line)
						tnode = 0;
				}

				if (!lines2.parent)
					lines2.parent = active_line;
			}
/* draw the currently selected item */

			if (edit_mode) {
				lines =	draw_item (active_line, (int) input, node, drawmode_edit);
			} else {
				lines =	draw_item (active_line, strlen(input), node, drawmode_completion);
			}

/* draw items below current item */
			{
				Node *prev_up = node;	/* to aid pgup/pgdn */
				Node *tnode = down (node, node);

				lines += active_line;
				if (lines >= LINES)
					tnode = 0;
				while (tnode) {
					if (!lines2.down && node_up (tnode) == node)
						lines2.down = lines - active_line;

					lines += draw_item (lines, 0, tnode, drawmode_normal);

					if (node_up (tnode) == prev_up) {
						hnb_nodes_down++;
						prev_up = tnode;
					}

					tnode = down (node, tnode);
					if (lines >= LINES)
						tnode = 0;

				}
			}
		}

	help_draw ();

	move (LINES - 1, COLS - 1);

	refresh ();

	hnb_nodes_up++;
	hnb_nodes_down++;
}

