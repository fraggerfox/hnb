/*
 * ui.c -- the part of hnb that calls curses
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

#include "tree.h"
#include "curses.h"
#include "stdio.h"
#include "prefs.h"
#define UI_C
#include "ui.h"

#ifdef NCURSES_MOUSE_VERSION
	#define MOUSE
#endif			
	#define hnbgimp
int nodes_above;
int middle_line;
int nodes_below;

#ifdef MOUSE
	#define mouse_maxnodes 100
	
	Node * mouse_node[mouse_maxnodes];
	int mouse_todo[mouse_maxnodes];
	int mouse_xstart[mouse_maxnodes];
	int mouse_xend[mouse_maxnodes];
	
	void mouse_clearnodes(){
	 	int j;
		for(j=0;j<mouse_maxnodes;j++){
			mouse_node[j]=(Node *)0;
			mouse_todo[j]=mouse_xstart[j]=mouse_xend[j]=0;
		}
	}
#endif

void ui_init (){
	initscr ();
	clear ();
	intrflush (stdscr, TRUE);
	keypad (stdscr, TRUE);
	nonl ();
	raw ();						/* enable the usage of ctl+c,ctrl+q,ctrl+z.. */	
	noecho ();
	middle_line = LINES / 3;
	#ifdef MOUSE
	if(prefs.mouse)
		mousemask(BUTTON1_CLICKED+BUTTON3_CLICKED+BUTTON1_DOUBLE_CLICKED,NULL);
	#endif	
	
	
	if (has_colors())
	{
		start_color();

		init_pair(UI_COLOR_MENUITM,	 prefs.fg_menuitm,prefs.bg_menuitm);
		init_pair(UI_COLOR_MENUTXT,	 prefs.fg_menutxt,prefs.bg_menutxt);
		init_pair(UI_COLOR_NODE,	 prefs.fg_node,prefs.bg_node);
		init_pair(UI_COLOR_NODEC,	 prefs.fg_nodec,prefs.bg_nodec);
		init_pair(UI_COLOR_BULLET,   prefs.fg_bullet,prefs.bg_bullet);
		init_pair(UI_COLOR_PRIORITY, prefs.fg_priority,prefs.bg_priority);
		init_pair(UI_COLOR_BG,		 COLOR_WHITE,prefs.bg);
	}
	/* COLS ? */
	nodes_above = middle_line;
	nodes_below = LINES - middle_line;

	bkgdset(' '+COLOR_PAIR(UI_COLOR_BG));
}

#define att_menuitem	{attrset(A_NORMAL);if(prefs.bold_menuitm)attron(A_BOLD);else attroff(A_BOLD);attron(COLOR_PAIR(UI_COLOR_MENUITM));}
#define att_menutext	{attrset(A_NORMAL);if(prefs.bold_menutxt)attron(A_BOLD);else attroff(A_BOLD);attron(COLOR_PAIR(UI_COLOR_MENUTXT));}
#define att_normal		{attrset(A_NORMAL);attron(COLOR_PAIR(UI_COLOR_BG));}

#define att_node		{if(prefs.bold_node)attron(A_BOLD);else attroff(A_BOLD);attron(COLOR_PAIR(UI_COLOR_NODE));}
#define att_nodec		{if(prefs.bold_nodec)attron(A_BOLD);else attroff(A_BOLD);attron(COLOR_PAIR(UI_COLOR_NODEC));}

#define att_bullet		{if(prefs.bold_bullet)attron(A_BOLD);else attroff(A_BOLD);attron(COLOR_PAIR(UI_COLOR_BULLET));}
#define att_priority	{if(prefs.bold_priority)attron(A_BOLD);else attroff(A_BOLD);attron(COLOR_PAIR(UI_COLOR_PRIORITY));}


Node *up (Node *sel,Node *node){
	switch(prefs.collapse_mode){
		case COLLAPSE_ALL:
		case COLLAPSE_ALL_BUT_CHILD:
			if (node_up (node))
				return (node_up (node));
			else
				return (node_left (node));
			break;
		case COLLAPSE_NONE:
			return node_backrecurse(node);
			break;
		case COLLAPSE_ONLY_SIBLINGS:
			return node_up(node);
			break;
		case COLLAPSE_PATH:
			if(nodes_left(node)==nodes_left(sel))
				if(node_up(node))return(node_up(node));
			return(node_left(node));
			break;			
	}
	return 0;
}

Node *down (Node *sel,Node *node){
	switch(prefs.collapse_mode){
		case COLLAPSE_ALL_BUT_CHILD:
			if((node==sel) && (node_right(node)))
				return node_right(node); 
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
			return node_recurse(node);
			break;
		case COLLAPSE_ONLY_SIBLINGS:
			return node_down(node);
			break;
		case COLLAPSE_PATH:
			return node_down(node);
			break;			
	}
	return (0);
}


/* this function draws a line at the top of the screen with various debug info,.. for tracing errors in the tree
   code, they seem to turn up every once in a while,.. but more and more seldomly
*/

void debug(Node *node,int mode){
	char buf[10];
#define debug_int(a,b,c)	att_menuitem;addstr(a);att_menutext;sprintf(buf,b,c);addstr(buf);

	
	move(0,0);
	debug_int("not","%.2i ",node_no(node));
	debug_int("id"," %.4X ",(int)node&0xFFFF);
	debug_int("up"," %.4X ",(int)node->up&0xFFFF);
	debug_int("dn"," %.4X ",(int)node->down&0xFFFF);
	debug_int("<-"," %.4X ",(int)node->left&0xFFFF);
	debug_int("->"," %.4X ",(int)node->right&0xFFFF);
	debug_int("lvl"," %.4X ",nodes_left(node));
	att_menuitem;addstr("flags");att_menutext;
	sprintf(buf," %i%i%i%i%i ",(node_getflags(node)>>4)&1,(node_getflags(node)>>3)&1,(node_getflags(node)>>2)&1,(node_getflags(node)>>1)&1,(node_getflags(node)>>0)&1);addstr(buf);		
	debug_int("pri"," %.1i ",node_getpriority(node));

	att_normal;
}

char toeleet(char in){
	char seta[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ(),0";
	char setb[]="aBCDeFGHiJKLMNoPQR$7uVWXyZ48(D3F6HiJKLMNoPQR57uVWXyZ[];o";
	int j;
	for(j=0;j<strlen(seta);j++){
		if(in==seta[j]){		
			return setb[j];
		}
	}
	return(in);
}

/*	help_draw is called with an mode, and an message that is used in some modes, see ui.h for the modes
	line - which line on the screen to draw on
	pos - which item number
	clr - clears the line
	clrc('a') - fills line with the character 'a'
	i("key","text") makes an entry at position specified by line and pos there are 6 positions per line
	rl("string") draws a string in reverse, spanning the whole line with the reverse box.
	mdc() clears all mouse mappings
	md(KEY_CODE) mouse define a keycode for the coords
*/

#ifdef MOUSE
	int mouse_codes[6][6];
	int mouse_warp=1;
	
	#define m_warp	{mouse_warp=1;}
	#define m_nwarp	{mouse_warp=0;}
	
	void mdc(){
		int pos,line;
		for(pos=0;pos<6;pos++)
			for(line=0;line<6;line++)
				mouse_codes[line][pos]=-1;
	}
	#define md(KEYCODE)	mouse_codes[(line-LINES)*-1][pos-1]=KEYCODE;
#endif

#ifndef MOUSE
	#define md(a)
	#define mdc()
	#define m_warp
	#define m_nwarp
#endif

void help_draw (int mode, char *message){
	int pos;
	int line;
	
	#define clrc(a) {int c;move(line,0);for(c=0;c<COLS;c++)addch(a);}
	#define clr	att_menutext;clrc(' ')att_normal;
	#define rl(a) att_menuitem;clrc(' ');move(line,0);\
				addstr(a);att_normal;
	#define i(KEY,TEXT) {move(line, pos * (COLS/6)  );att_menuitem;\
				addstr(KEY);att_menutext;addstr(TEXT);pos++;att_normal;}
	
	switch (mode) {
		case UI_MODE_CONFIRM:mdc();m_nwarp;
			line = LINES - 2;
			rl(message);
			line = LINES - 1;
			pos = 0;
			clr;
			i (" Y ", " yes");md('y');
			pos=5;			
			i ("Esc,C", " cancel");md('c');
			break;
		case UI_MODE_PREFS:
			line = LINES - 2;mdc();m_warp;
			rl("hnb configuration");
			line = LINES - 1;
			pos = 0;
			clr;
			i ("arrows", " move");
			i ("return", " change");md(UI_ENTER);
			pos++;
			i ("^A", " apply");md(UI_INSERT);
			i ("^S", " save");md(UI_REMOVE);
			i ("^X", " cancel");md(UI_QUIT);
			break;
		case UI_MODE_HELP2:mdc();m_warp;
			break;
		case UI_MODE_HELP0:		
			line = LINES - 1;mdc();m_warp;
			pos = 0;
			clr;
			i (" F1 ", " help");md(UI_HELP);
			i ("arrows", " move");
			i ("return", " edit");md(UI_ENTER);
			i ("ins", " insert");md(UI_INSERT);
			i ("del", " remove");md(UI_REMOVE);
			i ("^X", " quit");md(UI_QUIT);
			break;
		case UI_MODE_HELP1:
			line = LINES - 5;m_warp;
			pos = 0;
			clr;
			i ("hnb", VERSION);mdc();
			i ("", "");
			i ("", "");
			i ("^F", " search");md(UI_FIND);
			i ("^O", " sort");md(UI_SORT);
			i ("^V", " viewmode");md(22);
			line = LINES - 4;
			pos = 0;
			clr;
			i ("(c)", " Øyvind Kolås 2000/2001");
			i ("", "");
			i ("", "");
			i ("", "");
			i ("^P", " priority");md(UI_PRIORITY);
			i ("^L", " import");md(UI_IMPORT);			
			line = LINES - 3;
			pos = 0;
			clr;
			i ("", "");
			i ("", "");
			#ifndef WIN32
				i ("^space", " grab");md(UI_MARK);
			#endif
			#ifdef WIN32
				i (" ^G ", " grab");
			#endif
			i ("^T", " todo toggle");md(UI_TOGGLE_TODO);
			i ("^D", " done toggle");md(UI_TOGGLE_DONE);
			i ("^E", " export");md(UI_EXPORT);
			line = LINES - 2;
			pos = 0;
			clr;
			i ("F5", " prefs");;md(UI_PREFS);
			i ("abc..", " match");
			i ("TAB", " complete");md(UI_COMPLETE);
			i ("^B", " parentify");md(UI_LOWER);
			i ("^N", " childify");md(UI_RAISE);
			i ("^S", " save");md(UI_SAVE);
			line = LINES - 1;
			pos = 0;
			clr;
			i ("F1", " help");md(UI_HELP);
			i ("arrows", " move");
			i ("return", " edit");md(UI_ENTER);
			i ("ins", " insert");md(UI_INSERT);
			i ("del", " remove");md(UI_REMOVE);
			i ("^X", " quit");md(UI_QUIT);
			break;
		case UI_MODE_EDIT:
			line = LINES - 2;mdc();m_nwarp;
			pos = 0;
			clr;
			i ("abc.. ", " entry");
			line = LINES - 1;
			pos = 0;
			clr;
			i ("return", " confirm");md(UI_ENTER);
			pos++;
			i ("arrows", " move");			
			i ("^J", " join");md(UI_JOIN);
			i ("^S", " split");md(UI_SPLIT);
			pos=5;
			i ("esc,^X", " cancel");md(UI_ESCAPE);
			break;
		case UI_MODE_EDITR:
			line = LINES - 2;mdc();m_nwarp;
			pos = 0;
			clr;
			i ("abc.. ", " entry");
			line = LINES - 1;
			pos = 0;
			clr;
			i ("return", " confirm");md(UI_ENTER);
			pos++;
			i ("arrows", " move");			
			pos=5;
			i ("esc,^X", " cancel");md(UI_ESCAPE);
			break;
		case UI_MODE_QUIT:
			line=LINES-2;
			rl("Really quit hnb?");
			line = LINES - 1;mdc();m_nwarp;
			clr;
			pos=0;
			i (" X ", " Save and Quit");md('Y');
			i ("", "");md('Y');
			i (" S ", " Save");md('s');			
			i (" Q ", " Quit");md('Q');
			pos=5;			
			i ("Esc,C", " cancel");md('c');
			break;
		case UI_MODE_EXPORT:m_nwarp;
			line = 0;mdc();
			pos = 0;
			clr;
			i ("      ", "");
			line = 1;
			pos = 0;
			clr;
			i (" info ", " the data from the current level and down are exported, not higher levels");
			line = 2;
			pos = 0;
			clr;
			i ("      ", "");
			line = 3;
			rl("");
			line = LINES - 1;
			pos = 0;
			clr;
			i (" A ", " Ascii");md('a');
			i (" H ", " html");md('h');
			i (" X ", " xml");md('x');
			i (" G ", " general xml");md('g');
			i (" S ", " stylized xml");md('s');
			i (" | ", " pipe");md('|');
			pos=5;
			i ("Esc,C", " Cancel");md('c');
			line = LINES - 2;
			pos = 0;
			clr;
			rl ("Export menu select format");
	break;
		case UI_MODE_MARKED:
			line = LINES - 1;mdc();m_nwarp;
			pos = 0;
			clr;
			pos++;
			i ("arrows", " navigate");
			pos++;
			i ("space", " move");md(' ');
			i ("C Enter", " copy");md(UI_ENTER);
			i ("Esc,Q", " cancel");md('q');
			line = LINES - 2;
			pos = 0;
			clr;
			rl ("node grabbed, move destination");
			break;
		case UI_MODE_ERROR:
			mdc();
			line = 0;
			pos = 0;clr;
			i ("Error:", message);
			break;
		case UI_MODE_INFO:
			mdc();
			line = 0;
			pos = 0;clr;
			i ("Info:", message);
			break;
		case UI_MODE_GETSTR:
			line = LINES - 2;
			rl(message);
			line = LINES - 1;m_nwarp;
			pos = 1;
			clr;
			i ("", ">");
			break;
		case UI_MODE_IMPORT:
			line = 0;m_nwarp;
			pos = 0;
			clr;
			i ("      ",
			   " the imported file is inserted into the tree at the ");
			line = 1;
			pos = 0;
			clr;
			i (" info ", " bottom of the current level.");
			line = 2;
			rl("");
			line = LINES - 1;
			pos = 0;
			clr;
			i (" A ", " Ascii");md('a');
			i (" X ", " xml");md('x');
			i (" G ", " general xml");md('g');
			i (" S ", " stylized xml");md('s');
			pos=5;
			i ("Esc,C", " Cancel");md('c');
			line = LINES - 2;
			pos = 0;
			clr;
			rl ("Import menu select format");						
			break;
		case UI_MODE_SEARCH:
			line = LINES - 2;mdc();m_nwarp;
			pos = 0;
			clr;
			rl ("Searching for:");
			attrset(A_REVERSE);
			addch (' ');
			addstr (message);
			attrset(A_NORMAL);			
			line = LINES - 1;
			pos = 0;clr;
			i ("Enter,S", " stop");md('s');
			i ("N,space", " next");md(' ');
			pos=5;			
			i ("Esc,C", " cancel");md('c');
			break;
#ifdef hnbgimp
		case 11111:line=LINES-2;pos=0;clr;i(" hnb-gimp "," ver -0.1");addch
		(' ');addstr(message);i("", ""); i("", "");i("ESC,Q", " quit");i
		("arrows"," draw");i(" X "," toggle color");line=LINES-1;pos=0;clr;
		i("", "");i("", "");i ("","");i(" I "," invert image");i("", "");i
		(" C "," clear canvas"); 
		break;
#endif

	}
}

#define indentstart 4

int startlevel = 0;

#define indentlevel(a)	(nodes_left(a)-startlevel)*prefs.indent+indentstart

/*	draws the user interface,
	inputs: selected node,
	        contents of input buffer
		mode (as defined in ui.h)
*/

#define D_M_CHILD	1
#define D_M_TEST	4
#define D_M_EDIT	8

int hnb_edit_posup=0;			/*contains the cursor pos for up/down*/
int hnb_edit_posdown=0;			/*from here when in editing mode*/

/* returns the completion status a node should have*/
#define done_status(a)					(prefs.showpercent?\
					node_calc_complete (a)\
				:\
					node_getflag(a,F_todo)?\
						node_getflag(a,F_done)?\
							1000\
						:0\
					:-1)


/* 	draws a single item,

	starting at coordinates line_start, and col_start
	*data contains the data to be spat out, if any
	draw_mode:
		D_M_EDIT 
		D_M_TEST - dont actually draw, ( to calculate space needed )
		D_M_CHILD - node has children

	completion:
		0:     draw [ ]
		1..99: draw 50%
		100:   draw [X]
		-1: dont draw completion bullet

	cursor_pos:
		in D_M_EDIT: the position to draw in reverse video
		

	Returns the number of lines used*/
int draw_item (int line_start, int col_start, char *data, 
			int draw_mode, int completion, int cursor_pos, int priority){
	int lines_used = 1;
	int col_end = COLS;
	char word[200];		/* maximum displayed (not stored) word length */	
	int wpos=0;			/* position in word */
	int dpos=0;			/* position in data */
	int col=col_start;	/* current column*/
	int cx,cy;			/* curosr coordinates */
		
	#define cs_no	 0
	#define cs_yes   1
	#define cs_below 2
	#define cs_done  3
	int cursor= cs_no;

/* draw bullet */

	if( ! (draw_mode & D_M_TEST )){

		att_bullet;
		if(completion==-1){	
			move(line_start,col_start-2);	
			switch(prefs.bulletmode){
				case BULLET_NONE:
					break;
				case BULLET_PLUSMINUS:
					addch( (draw_mode & D_M_CHILD) ? '+':'-');
					break;
				case BULLET_STAR:
					addch('*');
					break;
				case BULLET_MINUS:
					addch('-');			
					break;
			}
		} else {
			move(line_start,col_start-4);
			switch(completion){
				case 0:
					addstr ("[ ]");
					break;
				case 1000:
					addstr ("[X]");
					break;
				default:{
					char str[10];
					sprintf (str, "%2i%%", completion / 10);
					addstr(str);
					}
			}
		}
		if(prefs.showpercent && priority && completion!=-1){
				att_priority;
				addch(priority+48);
		}

		
		
		
	}

	if(!(draw_mode & D_M_TEST)){
		if(draw_mode & D_M_CHILD){
			att_nodec;
		} else { 
			att_node;
		}
	}

	word[0]=0;
		
	if(draw_mode & D_M_EDIT){
		hnb_edit_posup=0;
		hnb_edit_posdown=strlen(data);
		cursor=cs_yes;
	}
		
	while((dpos==0) || data[dpos-1]){		/* do this on every char including term char*/
		switch(data[dpos]){
			case 0:
			case ' ':case 9:case 10:case 11: /* treat tabs and newlines as spaces */
				if(col+wpos + 1 >=col_end){	/*reached margin*/
					
					if(cursor==cs_yes){
						hnb_edit_posup = cursor_pos-(col-col_start);
					} else if(cursor==cs_below) {
						hnb_edit_posdown= cursor_pos+(col-col_start);
						cursor=cs_done;
					}

					col=col_start;
					lines_used++;

					if(lines_used+line_start >= LINES) /* reached bottom of term*/
						return lines_used;
				}
				if(!(draw_mode & D_M_TEST)){
					if(line_start+lines_used-1>=0){
						move(line_start+lines_used-1,col);
						addstr(word);
						if(data[dpos])addch(' ');
					}
					
					if(cursor==cs_yes && dpos>=cursor_pos){
						cy=line_start+lines_used-1;
						cx=col-(dpos-cursor_pos)+wpos;
						cursor=cs_below;
					}
				}
				col+=wpos+1;
				word[wpos=0]=0;
				break;
			default:
				if(wpos<198){
					word[wpos++]=data[dpos];
					word[wpos]=0;
				}
		}
	dpos++;
	}
	if((draw_mode & D_M_EDIT) && !(draw_mode & D_M_TEST)){
		move(cy,cx);attron(A_REVERSE);addch(data[cursor_pos]);attroff(A_REVERSE);
	}	
	return lines_used;
}

extern int hnb_nodes_up;
extern int hnb_nodes_down;

void ui_draw (Node *node, char *input, int mode){
	static int ignore_next=0;
	int lines;
	
	/*FIXME?: calculate .. startlevel,.. making the interface move right if the  nodes are really nested */
	
	if(!ignore_next)
		if (/*mode != UI_MODE_QUIT &&
			mode != UI_MODE_CONFIRM &&*/
			mode != UI_MODE_ERROR &&
			mode != UI_MODE_INFO &&
			/*mode != UI_MODE_GETSTR &&
			mode != UI_MODE_EXPORT &&
			mode != UI_MODE_IMPORT && */mode != UI_MODE_DEBUG)
	/* draw the nodes */
	{
	hnb_nodes_up=0;
	hnb_nodes_down=0;
	
		erase ();		
#ifdef MOUSE
	if(prefs.mouse)
		mouse_clearnodes();
#endif
/* draw nodes above selected node */
	{
		int line = middle_line;
			Node *tnode = up (node,node);

	while (tnode) {
		draw_item (line -=
			   draw_item (0, indentlevel (tnode),
				  node_getdata (tnode),
				  D_M_TEST, 
				  -1,
				  0,0),
				indentlevel (tnode),
				node_getdata (tnode),
				(node_right (tnode) ? D_M_CHILD : 0),
				done_status(tnode),
				0,
				node_getpriority(tnode));
#ifdef MOUSE						   
	if(prefs.mouse && line>0){
		mouse_node[line]=tnode;
		mouse_todo[line]=node_getflag(tnode,F_todo)?1:0;
		mouse_xstart[line]=indentlevel(tnode)+3;
	}
#endif
	tnode = up (node,tnode);
		if (middle_line - nodes_above >= line)
			tnode = 0;
			hnb_nodes_up++;
			}
		}

/* draw the selected node.. */

#ifdef MOUSE			
	if(prefs.mouse){			   
		mouse_node[middle_line]=node;
		mouse_todo[middle_line]=node_getflag(node,F_todo)?1:0;
		mouse_xstart[middle_line]=indentlevel(node)+3;
	}
#endif

	if(mode== UI_MODE_EDIT || mode == UI_MODE_EDITR){
		lines =draw_item (middle_line,
			indentlevel (node),
			node_getdata (node),
			D_M_EDIT+ (node_right (node) ? D_M_CHILD : 0),
			done_status(node),(int)input,
			node_getpriority(node));
	} else {				
		attrset (A_REVERSE);
			/* the selected node */
		lines = draw_item (middle_line,
				indentlevel (node),
				node_getdata (node),
				(node_right (node) ? D_M_CHILD : 0),
				-1, 0,0);
		attrset (A_BOLD);
		/* the search input */
		if (mode == UI_MODE_HELP0 || mode == UI_MODE_HELP1 || mode == UI_MODE_HELP2)
			draw_item (middle_line,
				indentlevel (node),
				input,
				0,
				-1,
				0,0);
		}
		attrset (A_NORMAL);

		/* bullet */
		draw_item (middle_line,
			indentlevel (node),
			"",
			(node_right (node) ? D_M_CHILD : 0),
			done_status(node),
			0,node_getpriority(node));
		

/* draw lines below selected node */
		{
		Node *tnode = down (node,node);

		lines += middle_line;

		while (tnode) {
#ifdef MOUSE						   
	if(prefs.mouse){
			mouse_node[lines]=tnode;
			mouse_todo[lines]=node_getflag(tnode,F_todo)?1:0;
			mouse_xstart[lines]=indentlevel(tnode)+3;
	}
#endif
			lines += draw_item (lines,
				indentlevel (tnode),
				node_getdata (tnode),
				(node_right (tnode) ? D_M_CHILD : 0),
				done_status(tnode),
				0,node_getpriority(tnode));
			tnode = down (node,tnode);
			if (middle_line + nodes_below <= lines)
				tnode = 0;
			hnb_nodes_down++;
		}
	}

#ifdef MOUSE
	if(prefs.mouse){
	int j;/*fill in the gaps in the lines */
	for(j=1;j<lines;j++){
		if(mouse_node[j]==0){
			if(mouse_node[j-1]!=0){
				mouse_node[j]=mouse_node[j-1];
				mouse_xstart[j]=mouse_xstart[j-1];
			}
		}
	}
	}
#endif
	}


	if(!ignore_next)help_draw (mode, input);

	if (mode == UI_MODE_GETSTR) {
		echo ();
		getstr (&input[0]);
		noecho ();
	}
	
	if(prefs.debug)
		debug(node,mode);
		
	move (LINES - 1, COLS - 1);	

	refresh ();
	ignore_next=0;
	if(mode==UI_MODE_INFO || mode==UI_MODE_ERROR)ignore_next=1;
}

void ui_end ()
{
	clear ();
	refresh ();
	endwin ();
}
	#ifdef hnbgimp
	#define pset(a,b,c) v[b][a]=c;
	#define pget(a,b) v[b][a];
	#define mva(a,b,c) mvaddch(a,b,c)
	#define cw 33
	#define ch 17
							char qixels[16] = " ,'L`/~Pcw\\b]d\\@"; int v[32][64];void cls () { int x, y; for (x = 0; x < 64; x++) for (y = 0; y < 32; y++) pset (x, y, 0); } void draw (int xp, int yp) { int x, y; for (x = 0; x < 16; x++) for (y = 0; y < 32; y++) { mva (x + yp + 1,y + xp + 1, qixels[v[x * 2][y * 2] * 2 + v[x * 2 + 1][y * 2] + v[x * 2][y * 2 + 1] * 4 + v[x * 2 + 1][y * 2 + 1] * 8]);} for (x =xp; x <= xp + cw; x++) { mva (yp, x, '_'); mva (yp + ch, x, '~'); } for (y = yp; y <= yp + ch; y++) { mva (y, xp + cw, '|'); mva(y, xp, '|'); } mva (yp, xp, '.'); mva (yp, xp + cw, '.'); mva (yp + ch, xp, '`'); mva (yp + ch, xp + cw, '\''); move (0, 0); }

#endif

extern Node *pos;

int ui_input ()
{
#ifdef MOUSE
	MEVENT mouse;
#endif

	int c;

	char * collapse_names[]={
	" collapse mode: all (standard)",
	" collapse mode: all but first level of children",
	" collapse mode: show whole tree",
	" collapse mode: show path of current level",
	""};

	c = getch ();
	switch (c) {
		case 22:
			prefs.collapse_mode++;
			if(prefs.collapse_mode>COLLAPSE_END)prefs.collapse_mode=0;
			ui_draw (pos, "", UI_MODE_HELP0 + prefs.help_level);			
			ui_draw(pos,collapse_names[prefs.collapse_mode],UI_MODE_INFO);
			return UI_IGNORE;
			break;
		case KEY_F(10):
			prefs.eleet_mode= !prefs.eleet_mode;
			return UI_IGNORE;
			break;
		case KEY_RESIZE:
			middle_line = LINES / 3;
			nodes_above = middle_line;
			nodes_below = LINES - middle_line;
			c = getch ();
			return UI_IGNORE;
			break;
#ifdef MOUSE			
	case KEY_MOUSE:
		getmouse(&mouse);
		if(mouse.bstate&BUTTON1_CLICKED 
			|| mouse.bstate&BUTTON1_DOUBLE_CLICKED){/*button1*/
			if (mouse.y >= LINES-5){
				int line=(mouse.y-LINES)*-1;
				int pos= (mouse.x/ (COLS/6));

				if( mouse_codes[line][pos]!= -1 ) 
					return mouse_codes[line][pos];
			}

		if(mouse_warp){
			if (mouse_node[mouse.y]!=0){
				if(mouse.x<mouse_xstart[mouse.y]-4){
					return(UI_LEFT);
				}
				if((mouse_todo[mouse.y]) 
						&& (mouse.x<mouse_xstart[mouse.y])){
					Node *tnode=mouse_node[mouse.y];
					node_toggleflag(tnode,F_done);
					node_update_parents_todo(tnode);
					return(UI_IGNORE);
				}
				if(mouse.x<mouse_xstart[mouse.y])
					return(UI_LEFT);
				if(mouse.x>=mouse_xstart[mouse.y]){
					if(mouse.y==middle_line){
						if(node_right(pos))
							pos=node_right(pos);
					} else {					
						pos=mouse_node[mouse.y];
						if(mouse.bstate&BUTTON1_DOUBLE_CLICKED)
							if(node_right(pos))
								pos=node_right(pos);
					}
				}
				return UI_IGNORE;
			  }

	  	if( mouse.y > middle_line ) 
			return UI_DOWN;
		if( mouse.y < middle_line ) 
		 	return UI_UP;			  
			  
		}
		}
		if(mouse.bstate&BUTTON3_CLICKED){/* button 3 */
			return(UI_HELP);
		}
		return UI_IGNORE;
		break;
#endif

#ifdef hnbgimp
		case KEY_F (12):{ int c = 0; int x = 5, y = 5, i = 1;erase (); help_draw (11111, ""); draw (2, 2); refresh (); while (c != 27) { c = getch (); switch (c) { case 8: case KEY_UP: y--; break; case 2: case KEY_DOWN: y++; break; case 4: case KEY_LEFT: x--; break; case 6: case KEY_RIGHT: x++;break; case 'i': case 'I':{ int x, y;for (y = 31; y >= 0; y--) for (x = 63; x >= 0; x--) { pset (x, y, !pget (x, y)); draw (2, 2); refresh ();} } case 'x': case 'X': i = !i; break; case 'c': cls (); break; case 'q': case KEY_F (12): c = 27; break; } x = x > 63 ?0 : x < 0 ? 63 : x; y = y > 31 ? 0 : y < 0 ? 31 : y; pset (x, y, i); pset (x, y, !pget (x, y)); draw (2, 2); refresh ();pset (x, y, !pget (x, y)); } return UI_IGNORE; }
#endif
	}

	return (c);
}
