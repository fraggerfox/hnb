/*
 * ui_overlay.c -- help and status display for hnb
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
 
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "tree.h"
#include "ui.h"
#include "ui_style.h"
#include <string.h>
#include "ui_binding.h"
#include "evilloop.h"

/*	ui_overlay
	
	current node
	dirtyness
	
	scope_no (and thus name and help texts)
	
	status
	status_display_counter,..

*/

/*
!cli cli_add_command ("helptext", ui_helptext_cmd, "<help for context>");
!clid int ui_helptext_cmd ();
*/

static char* ui_helptext[MAX_SCOPES]={0};

int ui_helptext_cmd(char *params,void *data){
	ui_helptext[ui_current_scope]=strdup(params);
	return (int)data;
}

#define MAX_STATUS_LINES 12

static char status_line[MAX_STATUS_LINES][128]={""};

static int status_ttl=0;

static void status(char *message, int ttl){
	int i;
	for(i=0;i<MAX_STATUS_LINES-1;i++)
		strncpy(status_line[i],status_line[i+1],128);
	strncpy(&status_line[MAX_STATUS_LINES-1][0],message, 128);
	status_ttl+=ttl;
	if(status_ttl>=MAX_STATUS_LINES)
		status_ttl=MAX_STATUS_LINES-1;
}

void set_status(char *message){
	status(message,1);
}

/*
!cli cli_add_command ("status", ui_status_cmd, "<message>");
!cli cli_add_command ("status_clear", ui_status_clear_cmd, "");

!clid int ui_status_cmd ();
!clid int ui_status_clear_cmd ();

*/

int ui_status_cmd(char *params,void *data){
	status(params,1);
	return (int)data;
}


int ui_status_clear_cmd(char *params,void *data){
	status_ttl=0;
	return (int)data;
}

void status_draw(void)
{
	int j;
	for(j=0;j<status_ttl;j++){
		move(status_ttl-j-1,0);
		ui_style(ui_style_menuitem);
		addstr(" ");
		ui_style(ui_style_background);
		addstr(" ");
		ui_style(ui_style_menutext);
		addstr(status_line[MAX_STATUS_LINES-j-1]);
		move(status_ttl-j-1,strlen(status_line[MAX_STATUS_LINES-j-1])+2);
		clrtoeol();
	}
	if(status_ttl>0)
		status_ttl--;
}

void help_draw (int scope)
{
	status_draw();

	move(LINES-1,0);
	ui_style(ui_style_menuitem);
	{unsigned char *p=ui_helptext[scope];
	 int style_is_menuitem=1;
 	 while(*p){
	 	switch(*p){
			case '|':
				if(*(p+1)=='|'){
					addch('|');
					p++;
				} else {					
					if(style_is_menuitem){
						ui_style(ui_style_menutext);
					} else {
						ui_style(ui_style_menuitem);
					}
					style_is_menuitem=!style_is_menuitem;					
				}
				break;
			default:
				addch(*p);
				break;
		}
		p++;
	 }
	}	


	clrtoeol();
	ui_style(ui_style_background);
	
}
