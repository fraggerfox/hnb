/*
 * prefs.c -- loading, keeping and saving of preferences
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
#define PREFS_C

#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "ui.h"
#include "registry.h"
#include "path.h"
#include "prefs.h"
#include "file.h"
#include "ctype.h"
#include "curses.h"	/*for color values*/

Tprefs prefs={
0,		/*default_help_level;*/
0,		/*help_level;*/
0,		/*forced_up;*/
0,		/*forced_down;*/
0,		/*usertag*/
{""},/*usertags;*/
COLLAPSE_ALL,	/*default_collapse_mode;*/
COLLAPSE_ALL,	/*collapse_mode;*/
FORMAT_HNB,		/*def_format;*/
FORMAT_HNB,		/*format;*/
0,		/*xml_cuddle;*/
0,		/*xml_highlight;*/
0,		/*tutorial;*/
0,		/*eleet_mode;*/
0,		/*debug;	*/
"",		/*rc_file[100];*/
"",		/*db_file[100];*/
"",		/*default_db_file[100];*/
COLOR_YELLOW,	/*fg_menu,*/
COLOR_BLACK,	/*bg_menu,*/
COLOR_BLACK,	/*bg,*/
COLOR_WHITE,	/*fg_node,*/
COLOR_BLACK,	/*bg_node */
COLOR_WHITE,	/*fg_nodec,*/
COLOR_BLACK,	/*bg_nodec */
COLOR_CYAN,		/*fg_bullet,*/
COLOR_BLACK		/*bg_bullet */
};

typedef struct{
	char name[4];
	int color;
}ColornameT;

ColornameT colorname[]={
	{"bla",COLOR_BLACK},
	{"blu",COLOR_BLUE},
	{"cya",COLOR_CYAN},
	{"red",COLOR_RED},
	{"gre",COLOR_GREEN},
	{"mad",COLOR_MAGENTA},
	{"yel",COLOR_YELLOW},
	{"whi",COLOR_WHITE},
	{"",0}
};

int name2color(const char *name_in){
	char name[10];
	int j;
	strncpy(name,name_in,10);
	name[9]=0;
	j=0;
	while(colorname[j].name[0]){
		if(strstr(name,colorname[j].name))
			return colorname[j].color;
		j++;
	}
	return(COLOR_WHITE);
}

void init_prefs(){
#ifndef WIN32
	sprintf (prefs.rc_file, "%s/.hnbrc", getenv ("HOME"));
	sprintf (prefs.default_db_file, "%s/.hnb", getenv ("HOME"));
#endif
#ifdef WIN32
	sprintf (prefs.rc_file, "C:\\hnb.rc");
	sprintf (prefs.default_db_file, "C:\\hnb_data");	
#endif
}

Node *load_prefs(){
	Node *tnode;
	tnode=tree_new();
	tnode=hnb_import(tnode,prefs.rc_file);
	return tnode;
}

void save_prefs(Node *node){
	apply_prefs(node);
	hnb_export ((Node *) node_root (node), prefs.rc_file);
}

void write_def_rc(){
	FILE *file;
	file=fopen(prefs.rc_file,"w");
	fprintf(file,
	#include "hnbrc.inc"
	);
	fclose(file);
}

/* FIXME add a function that loads user prefs from file, and 
   rewrites with in memory xml tree
   
   (should just be called as)
   load_prefs;
   apply_prefs;
   write_def_rc;
   load_prefs;
   prefs_apply; (apply prefs to tree)
   save_prefs; )
   
   this would make it possible to add new features, and actually remove
   features, without the user noticing it. The hnbrc file is not
   a place to hack anyways, just an internal representation of hnbs
   configuration, with the ui in place.
*/

void apply_prefs(Node *node){
	char color[20];

	#define radio(a,b,c)	if(node_getflag( matchpath2node(a,node_root(node)),F_done))	b=c;
	#define check(a,b)		b=(node_getflag( matchpath2node(a,node_root(node)),F_done))?1:0;
	#define string(a,b)		strcpy(b,node_getdata(matchpath2node(a,node_root(node))));
	#define string2(a,b)	strcpy(b,node_getdata(node_down(matchpath2node(a,node_root(node)))));
	
	radio("/int/help/spar",prefs.help_level,0);
	radio("/int/help/full",prefs.help_level,1);
	radio("/int/help/exper",prefs.help_level,2);	

	radio("/int/tree/def/coll",prefs.collapse_mode,0);
	radio("/int/tree/def/show c",prefs.collapse_mode,1);
	radio("/int/tree/def/show w",prefs.collapse_mode,2);	
	radio("/int/tree/def/show p",prefs.collapse_mode,3);

/* colors */	
	string( "/int/col/back/",color);prefs.bg=name2color(color);
	string( "/int/col/men/",color);prefs.fg_menu=name2color(color);
	string2("/int/col/men/",color);prefs.bg_menu=name2color(color);
	string( "/int/col/node witho/",color);prefs.fg_node=name2color(color);
	string2("/int/col/node witho/",color);prefs.bg_node=name2color(color);
	string( "/int/col/node with /",color);prefs.fg_nodec=name2color(color);
	string2("/int/col/node with /",color);prefs.bg_nodec=name2color(color);
	string( "/int/col/bull/",color);prefs.fg_bullet=name2color(color);
	string2("/int/col/bull/",color);prefs.bg_bullet=name2color(color);

	check("/nav/forced up",prefs.forced_up);
	check("/nav/forced do",prefs.forced_down);

/*usertags*/
	check(	"/edit/usert",prefs.usertag);

	{int j;
	char str[20];
	for(j=1;j<27;j++){
		sprintf(str,"/edit/usert/^%c/",j+64);
		string(str,prefs.usertags[j]);
		}
	}

	radio("/file/default file format/xml",prefs.def_format,FORMAT_XML);
	radio("/file/default file format/hnb",prefs.def_format,FORMAT_HNB);
	radio("/file/default file format/asc",prefs.def_format,FORMAT_ASCII);	

	check("/file/xml/cudd",prefs.xml_cuddle);

	check("/misc/debug",prefs.debug);
	
	prefs.def_help_level=prefs.help_level;
	prefs.def_collapse_mode=prefs.collapse_mode;
	
	ui_init();
	ui_end();
}
