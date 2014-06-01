/*
 * prefs.c -- preferences and global variable mangement of hnb
 *
 * Copyright (C) 2001,2003 Øyvind Kolås <pippin@users.sourceforge.net>
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
#include "path.h"
#include "prefs.h"
#include "cli.h"
#include "ui_cli.h"
#include "file.h"

const char *format_name[format_terminator] = {
	"hnb", "ascii", "xml", "sxml", "html", "slides", "help"
};


Tprefs prefs = {
	0,							/*default_help_level; */
	0,							/*help_level; */
	0,							/*forced_up; */
	0,							/*forced_down; */
	COLLAPSE_ALL,				/*default_collapse_mode; */
	COLLAPSE_ALL,				/*collapse_mode; */
	format_hnb,					/*def_format; */
	format_hnb,					/*format; */
	0,							/*xml_cuddle; */
	0,							/*xml_highlight; */
	0,							/*tutorial; */
	0,							/*debug;    */
	"",							/*rc_file[100]; */
	"",							/*db_file[100]; */
	"",							/*default_db_file[100]; */
	"",							/*query*/
	4,							/*indent    */
	BULLET_NONE,				/*bulletmode */
	1,							/*showpercent */
	0,							/*keep whitespace */
	0,							/*fixed focusbar */
	0							/*save position */
};

typedef struct {
	char name[4];
	int color;
} ColornameT;


void init_prefs ()
{
#ifndef WIN32
	sprintf (prefs.rc_file, "%s/.hnbrc", getenv ("HOME"));
	sprintf (prefs.default_db_file, "%s/.hnb", getenv ("HOME"));
#endif
#ifdef WIN32
	sprintf (prefs.rc_file, "C:\\hnb.rc");
	sprintf (prefs.default_db_file, "C:\\hnb_data");
#endif

cli_add_int("help_level", &prefs.help_level, "amount of help cluttering screen" );
cli_add_int("forced_up",  &prefs.forced_up,  "wether movement upwards is forced beyond first sibling" );
cli_add_int("forced_down",&prefs.forced_down,"wether movement downwards is forced beyond last sibling" );

cli_add_int("collapse_mode",&prefs.collapse_mode,"");

cli_add_int("xml_cuddle",&prefs.xml_cuddle,"");
cli_add_int("xml_highlight",&prefs.xml_highlight,"");

cli_add_int ("helplvl", &prefs.help_level,  "level og help provided to user");
cli_add_int ("debug", &prefs.debug, "view debug information");
/*str*/cli_add_int ("format", &prefs.format, "the format of this file");
cli_add_int ("def_format", &prefs.def_format, "default format (and format of default db)");

cli_add_string("rc_file",prefs.rc_file,"");
cli_add_string("db_file",prefs.db_file,"");
cli_add_string("default_db_file",prefs.default_db_file,"");
cli_add_string("query",prefs.query,"");


cli_add_int("indentspaces",&prefs.indent,"");
cli_add_int("bulletmode",&prefs.bulletmode,"");
cli_add_int("showpercent",&prefs.showpercent,"");
cli_add_int("keepwhitespace",&prefs.keepwhitespace,"");
cli_add_int("fixedfocus",&prefs.fixedfocus,"");
cli_add_int("savepos",&prefs.savepos,"");

#ifdef NCURSES_VERSION

cli_add_int("escdelay",&ESCDELAY,"curses variable");

#endif
}

void write_default_prefs ()
{
	FILE *file;

	file = fopen (prefs.rc_file, "w");
	fprintf (file,
		#include "hnbrc.inc"
		);
	fclose (file);
}

void load_prefs (char *prefsfile)
{
	if(xml_check(prefs.rc_file)){
	printf("seems like your current ~/.hnbrc is outdated (it's xml the new format\n\
is plain text,.. remove it and let hnb make a new default\n");
	exit(0);
	}
	cli_load_file(prefs.rc_file);
}

