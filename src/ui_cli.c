/*
 * ui_cli.c -- the glue that binds various modules into the interpreter
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

#include "tree.h"
#include "file.h"
#include "path.h"
#include "prefs.h"
#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_cli.h"


/* strips the ending node off a path */
static char *path_strip (char *path)
{								/* FIXME add double // escaping when node2path gets it */

	int j = strlen (path);

	while (j > 0) {
		if (path[j - 1] == '/') {
			path[j] = 0;
			return path;
		}
		j--;
	}
	return path;
}

static int add (char *params, void *data)
{
	Node *pos = (Node *) data;
	Node *tnode;

	if (!params[0]) {
		cli_outfun("empty node added\n");
	}

	tnode = node_insert_down (node_bottom (pos));
	node_setdata (tnode, params);
	return (int) pos;
}

static int addc (char *params, void *data)
{
	Node *pos = (Node *) data;
	Node *tnode;

	int j = 0;
	char par[100];
	char *child;

	strncpy (par, params, 100);
	par[99] = ' ';
	while (par[j] != ' ' && par[j] != 0)
		j++;
	par[j] = 0;

	child = &params[j + 1];
	if (strlen (par) == strlen (params))
		child = strdup ("");

	tnode = node_exact_match (par, pos);
	if (!tnode) {
		cli_outfun("specified parent not found");
		return (int) pos;
	}
	if (node_right (tnode)) {
		cli_outfun("failed, node already had a child");
		return (int) pos;
	}

	if (!child[0]) {
		cli_outfun("empty node added\n");
	}

	tnode = node_insert_right (tnode);
	node_setdata (tnode, child);

	return (int) pos;
}

static int pwd (char *params, void *data)
{
	Node *pos = (Node *) data;

	cli_outfun(path_strip (node2path (pos)));
	cli_outfun("\n");
	return (int) pos;
}

static int cd (char *params, void *data)
{
	Node *pos = (Node *) data;
	Node *tnode = pos;

	if (!strcmp (params, ".."))
		params[0] = 0;

	if (params[0]) {
		tnode = path2node (params, pos);
		if (tnode) {
			tnode = node_right (tnode);
		}
		if (!tnode) {
			cli_outfun("no such node\n");
			return (int) pos;
		}
		return (int) tnode;
	} else {					/* go to parent */
		if (node_left (tnode) != 0)
			return (int) (node_left (tnode));
	}

	return (int) pos;
}


static int rm (char *params, void *data)
{
	Node *pos = (Node *) data;
	Node *tnode;
	int force = 0;

	if (params[0] == '-') {		/* parse options */
		int j = 0;

		while (params[j] != 0 && params[j] != ' ') {
			j++;
			if (params[j] == 'H' || params[j] == 'h') {
				cli_outfun("type '? rm' for help on rm");
				return (int) pos;
			}
			if (params[j] == 'F' || params[j] == 'f' || params[j] == 'r')
				force = 1;
		}
		params = &params[j];
		if (params[0] == ' ')
			params++;
	}

	if (!params[0]) {
		cli_outfun("no node specified for removal");
		return (int) pos;
	}

	tnode = path2node (params, pos);
	if (!tnode) {
		cli_outfun("no such node");
		return (int) pos;
	}

	if (node_right (tnode)) {
		if (force) {
			return (int) node_remove (tnode);
		} else {
			cli_outfun("node has children force removal with 'rm -f'");
		}
	} else {
		return (int) node_remove (tnode);
	}

	return (int) pos;
}

static int ls (char *params, void *data)
{
	Node *pos = (Node *) data;

	Node *tnode;
	int recurse = 0;
	int indicate_sub = 0;
	int indicate_todo = 0;
	int startlevel;

	if (params[0] == '-') {		/* parse options */
		int j = 0;

		while (params[j] != 0 && params[j] != ' ') {
			j++;
			if (params[j] == 'H' || params[j] == 'h') {
				cli_outfun("type '? ls' for help on ls");
				return (int) pos;
			}
			if (params[j] == 'R' || params[j] == 'r')
				recurse = 1;
			if (params[j] == 'T' || params[j] == 't')
				indicate_todo = 1;
			if (params[j] == 's' || params[j] == 'S')
				indicate_sub = 1;
		}
		params = &params[j];
		if (params[0] == ' ')
			params++;
	}

	tnode = node_top (pos);
	if (params[0]) {
		tnode = path2node (params, pos);
		if (tnode) {
			tnode = node_right (tnode);
		} else {
			cli_outfun( "no such node");
			return (int) pos;
		}
	}

	startlevel = nodes_left (tnode);
	while (tnode) {
		int paren = 0;

		if (recurse) {
			int j;

			for (j = nodes_left (tnode); j > startlevel; j--) {
				printf ("\t");
			}
		}

		cli_outfun( node_getdata (tnode));

		if (indicate_sub)
			if (node_right (tnode)) {
				cli_outfun( "\t(.. ");
				paren = 1;
			}

		if (indicate_todo)
			if (node_getflag (tnode, F_todo)) {
				if (!paren)
					cli_outfun( "\t(");
				if (node_getflag (tnode, F_done)) {
					cli_outfun( "done");
				} else {
					cli_outfun( "not done");
				}
				paren = 1;
			}

		if (paren)
			cli_outfun( ")");

		if (recurse) {
			tnode = node_recurse (tnode);
			if (nodes_left (tnode) < startlevel)
				tnode = 0;
		} else {
			tnode = node_down (tnode);
		}
	}
	return (int) pos;
}

#include <ctype.h>

void pre_command(char *commandline){
	char *c=commandline;
	if(commandline){ 
		while(isspace(*c))c++;
		if(*c=='#')commandline[0]='\0';
		if(*c=='\0')commandline[0]='\0';
	}
}

static int eCho(char *commandline, void *data){
	cli_outfun(commandline);
	return (int)data;
}

/*
!init_ui_cli();
*/
void init_ui_cli (void)
{
	static int inited = 0;
	if (!inited) {
		inited = 1;
		cli_precmd=pre_command;
		cli_add_command ("add", add, "<string>");
		cli_add_command ("addc", addc, "<parent> <string>");
		cli_add_command ("cd", cd, "<path>");
		cli_add_command ("ls", ls, "[-Rst] [path]");
		cli_add_command ("rm", rm, "[-f] path");
		cli_add_command ("pwd", pwd,"");
		cli_add_help("pwd", "echoes the current path");
		cli_add_command ("q", eCho, "see the entry for 'quit'");
	}
}

Node *docmd (Node *pos, const char *commandline)
{
	int ret;
	char *cmdline = strdup (commandline);

	ret = cli_docmd (cmdline, pos);
	free (cmdline);
	return (Node *) ret;
}

Node *cli (Node *pos)
{
	char commandline[4096];

	fprintf (stderr,
			 "Welcome to %s %s\ntype ? or help for more information\n",
			 PACKAGE, VERSION);

	do {
		fflush (stdout);
		fprintf (stdout, "%s>", path_strip (node2path (pos)));
		fflush (stdout);
		fgets (commandline, 4096, stdin);
		commandline[strlen (commandline) - 1] = 0;
		pos = (Node *) cli_docmd (commandline, pos);
	} while (strcmp (commandline, "quit") && strcmp (commandline, "q"));
	return pos;
}
