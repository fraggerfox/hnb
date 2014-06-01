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
COLOR_BLACK,	/*fg_menuitm,*/
COLOR_YELLOW,	/*bg_menuitm,*/
0,				/*bold		*/
COLOR_YELLOW,	/*fg_menutxt,*/
COLOR_BLACK,	/*bg_menutxt,*/
0,				/*bold		*/
COLOR_BLACK,	/*bg,*/
COLOR_WHITE,	/*fg_node,*/
COLOR_BLACK,	/*bg_node */
0,				/*bold		*/
COLOR_WHITE,	/*fg_nodec,*/
COLOR_BLACK,	/*bg_nodec */
0,				/*bold		*/
COLOR_CYAN,		/*fg_bullet,*/
COLOR_BLACK,	/*bg_bullet */
COLOR_RED,		/*fg_priority,*/
COLOR_BLACK,	/*bg_priority */
0,				/*bold prior*/
0,				/*bold_bul	*/
0,				/*rc revision*/
1,				/*mouse		*/
4,				/*indent	*/
BULLET_NONE,	/*bulletmode*/
1				/*showpercent*/
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
	{"mag",COLOR_MAGENTA},
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

	#define getint(a,b)		b=atoi(node_getdata(matchpath2node(a,node_root(node))));
	
	radio("/int/help/spar",prefs.help_level,0);
	radio("/int/help/full",prefs.help_level,1);
	radio("/int/help/exper",prefs.help_level,2);	

	radio("/int/tree/def/coll",prefs.collapse_mode,COLLAPSE_ALL);
	radio("/int/tree/def/show c",prefs.collapse_mode,COLLAPSE_ALL_BUT_CHILD);
	radio("/int/tree/def/show w",prefs.collapse_mode,COLLAPSE_NONE);
	radio("/int/tree/def/show p",prefs.collapse_mode,COLLAPSE_PATH);

	radio("/int/tree/bull/none",prefs.bulletmode,BULLET_NONE);
	radio("/int/tree/bull/+",prefs.bulletmode,BULLET_PLUSMINUS);
	radio("/int/tree/bull/*",prefs.bulletmode,BULLET_STAR);
	radio("/int/tree/bull/-",prefs.bulletmode,BULLET_MINUS);
	
	check("/int/tree/bull/show",prefs.showpercent);
	getint("/int/tree/indent/",prefs.indent);	

/* colors */	
	string( "/int/col/back/",color);prefs.bg=name2color(color);
	string( "/int/col/menu item/",color);prefs.fg_menuitm=name2color(color);
	string2("/int/col/menu item/",color);prefs.bg_menuitm=name2color(color);
	check("/int/col/menu item/bold",prefs.bold_menuitm);
	string( "/int/col/menu text/",color);prefs.fg_menutxt=name2color(color);
	string2("/int/col/menu text/",color);prefs.bg_menutxt=name2color(color);
	check("/int/col/menu text/bold",prefs.bold_menutxt);
	string( "/int/col/node witho/",color);prefs.fg_node=name2color(color);
	string2("/int/col/node witho/",color);prefs.bg_node=name2color(color);
	check("/int/col/node witho/bold",prefs.bold_node);
	string( "/int/col/node with /",color);prefs.fg_nodec=name2color(color);
	string2("/int/col/node with /",color);prefs.bg_nodec=name2color(color);
	check("/int/col/node with/bold",prefs.bold_nodec);
	string( "/int/col/bull/",color);prefs.fg_bullet=name2color(color);
	string2("/int/col/bull/",color);prefs.bg_bullet=name2color(color);
	check("/int/col/bull/bold",prefs.bold_bullet);
	string( "/int/col/pri/",color);prefs.fg_priority=name2color(color);
	string2("/int/col/pri/",color);prefs.bg_priority=name2color(color);
	check("/int/col/bull/bold",prefs.bold_priority);


	check("/nav/forced up",prefs.forced_up);
	check("/nav/forced do",prefs.forced_down);
	check("/nav/mouse",prefs.mouse);

	radio("/file/default file format/xml",prefs.def_format,FORMAT_XML);
	radio("/file/default file format/hnb",prefs.def_format,FORMAT_HNB);
	radio("/file/default file format/asc",prefs.def_format,FORMAT_ASCII);	
	radio("/file/default file format/lib",prefs.def_format,FORMAT_LIBXML);	

	check("/file/xml/cudd",prefs.xml_cuddle);

	check("/misc/debug",prefs.debug);

	getint("/misc/rc rev/",prefs.rc_rev);


	getint("/int/keyb/gen/help/",KEY.HELP);
	getint("/int/keyb/gen/cancel/",KEY.CANCEL);
	getint("/int/keyb/gen/edit/",KEY.CONFIRM);

	getint("/int/keyb/gen/top/",KEY.TOP);
	getint("/int/keyb/gen/bottom/",KEY.BOTTOM);
	getint("/int/keyb/gen/left/",KEY.LEFT);
	getint("/int/keyb/gen/right/",KEY.RIGHT);
	getint("/int/keyb/gen/up/",KEY.UP);
	getint("/int/keyb/gen/down/",KEY.DOWN);
	getint("/int/keyb/gen/skip up/",KEY.SKIP_UP);
	getint("/int/keyb/gen/skip down/",KEY.SKIP_DOWN);
	
	getint("/int/keyb/oth/toggle view/",KEY.TOGGLE_VIEWMODE);
	getint("/int/keyb/oth/search/",KEY.FIND);
	getint("/int/keyb/oth/pref/",KEY.PREFS);
	
	getint("/int/keyb/node/grab/",KEY.GRAB);
	getint("/int/keyb/node/sort/",KEY.SORT);
	getint("/int/keyb/node/inse/",KEY.INSERT);
	getint("/int/keyb/node/remo/",KEY.REMOVE);
	getint("/int/keyb/node/set pri/",KEY.SET_PRIORITY);
	getint("/int/keyb/node/toggle do/",KEY.TOGGLE_DONE);
	getint("/int/keyb/node/toggle to/",KEY.TOGGLE_TODO);
	getint("/int/keyb/node/child/",KEY.CHILDIFY);
	getint("/int/keyb/node/parent/",KEY.PARENTIFY);
		
	getint("/int/keyb/file/save/",KEY.SAVE);
	getint("/int/keyb/file/expo/",KEY.EXPORT);
	getint("/int/keyb/file/impo/",KEY.IMPORT);
	getint("/int/keyb/file/quit/",KEY.QUIT);
	
	getint("/int/keyb/edit/join/",KEY.JOIN);
	getint("/int/keyb/edit/split/",KEY.SPLIT);

/*usertags*/
	check(	"/int/keyb/edit/usert",prefs.usertag);

	{int j;
	char str[20];
	for(j=1;j<27;j++){
		sprintf(str,"/int/keyb/edit/usert/^%c/",j+64);
		string(str,prefs.usertags[j]);
		}
	}
	
	prefs.def_help_level=prefs.help_level;
	prefs.def_collapse_mode=prefs.collapse_mode;
	
	ui_init();
	ui_end();
}
