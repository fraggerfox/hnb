/*
 * ui.c -- management of colors and attributes for hnb
 *
 * Copyright (C) 2003 �yvind Kol�s <pippin@users.sourceforge.net>
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

#include <string.h>
#include "tree.h"
#include "prefs.h"
#include "ui.h"
#include "ui_style.h"

#include "cli.h"  

/*
!cli cli_add_command ("style", ui_style_cmd, "<item> <foreground/background> <attributes>");
!clid int ui_style_cmd ();
*/


typedef struct {
	char name[16];
	int att;
} styleitm;

static styleitm styledb[]={
	{"reserved", 0},
	{"menuitem",   A_REVERSE},
	{"menutext",   A_NORMAL},
	{"node",       A_NORMAL},
	{"parentnode", A_BOLD},
	{"bullet",     A_NORMAL},
	{"selected",   A_REVERSE},
	{"parentselected", A_REVERSE|A_BOLD},
	{"background", A_NORMAL},
};

void ui_style(int style_no){
	attrset(styledb[style_no].att);
}

typedef struct {
	char name[4];
	int color;
} ColornameT;

static ColornameT colorname[] = {
	{"bla", COLOR_BLACK},
	{"blu", COLOR_BLUE},
	{"cya", COLOR_CYAN},
	{"red", COLOR_RED},
	{"gre", COLOR_GREEN},
	{"mag", COLOR_MAGENTA},
	{"yel", COLOR_YELLOW},
	{"whi", COLOR_WHITE},
	/* 20021129 RVE - default terminal colors */
#ifdef NCURSES_VERSION
    {"def", -1},
#endif
	{"", 0}
};

static int name2color (const char *name_in)
{
	char name[10];
	int j;

	strncpy (name, name_in, 10);
	name[9] = 0;
	j = 0;
	while (colorname[j].name[0]) {
		if (strstr (name, colorname[j].name))
			return colorname[j].color;
		j++;
	}

#ifdef NCURSES_VERSION
	return -1;
#endif

	return (COLOR_WHITE);
}

static int string2style(char *str){
	int j=0;
	while(j<ui_style_terminator){
		if(!strcmp(styledb[j].name,str))
			return j;
		j++;
	}
	return -1;
}

int ui_style_cmd(char *params, void *data){
	char item[40];
	char colors[40];
	char atts[40];
	char *tail;

	tail=params;
	while(*tail==' ' || *tail=='\t')tail++;
	cli_split(tail,item,&tail);
	while(*tail==' ' || *tail=='\t')tail++;
	cli_split(tail,colors,&tail);
		while(*tail==' ' || *tail=='\t')tail++;
	cli_split(tail,atts,&tail);

	{
		int style_no=string2style(item);
		char *color2;
		if(style_no==-1){
			printf("unknown style 'style [%s] %s %s'\n",item,colors,atts);
			return (int)data;
		}
		color2=strchr(colors,'/');
		color2[0]='\0';
		color2++;

		init_pair(style_no, name2color(colors),name2color(color2));
		styledb[style_no].att=A_NORMAL | COLOR_PAIR(style_no);

		if(strstr(atts,"standout"))styledb[style_no].att |=  A_STANDOUT;
		if(strstr(atts,"underline"))styledb[style_no].att |= A_UNDERLINE;
		if(strstr(atts,"reverse"))styledb[style_no].att |=   A_REVERSE;
		if(strstr(atts,"blink"))styledb[style_no].att |=     A_BLINK;
		if(strstr(atts,"dim"))styledb[style_no].att |=       A_DIM;
		if(strstr(atts,"bold"))styledb[style_no].att |=      A_BOLD;

		if(style_no==ui_style_background){

			bkgdset (' ' + COLOR_PAIR (ui_style_background));
		}
	}
	return (int)data;
}
