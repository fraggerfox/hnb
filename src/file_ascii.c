/*
 * file_ascii.c -- ascii import and export filters for hnb
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

/*
!cli cli_add_command ("export_ascii", export_ascii, "<filename>");
!cli cli_add_command ("import_ascii", import_ascii, "<filename>");

!clid int import_ascii ();
!clid int export_ascii ();
*/


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "cli.h"
#include "tree.h"
#include "file.h"

#define indent(count,char)	{int j;for(j=0;j<count;j++)fprintf(file,char);}


int import_ascii (char *params, void *data)
{
	Node *node = (Node *) data;
	char *filename = params;
	int level, flags, cnt;
	import_state_t ist;
	char cdata[bufsize];
	FILE *file;

	file_error[0] = 0;
	file = fopen (filename, "r");
	if (file == NULL) {
		sprintf (file_error, "ascii import, unable to open \"%s\"", filename);
		return (int) (node);
	}

	init_import (&ist, node);

	while (fgets (cdata, bufsize, file) != NULL) {
		flags = level = cnt = 0;

		/*strip newlines and carrier return  */
		while (cdata[strlen (cdata) - 1] == 13
			   || cdata[strlen (cdata) - 1] == 10)
			cdata[strlen (cdata) - 1] = 0;

		while (cdata[level] == '\t')	/* find the level of this node */
			level++;

		if (cdata[level] == '[') {	/* read the flags */
			while (cdata[level + cnt] != ']') {
				cnt++;
				switch (cdata[level + cnt]) {
					case ' ':
						flags = flags + F_todo;
						break;
					case 'x':
					case 'X':
					case '*':
						flags = flags + F_todo + F_done;
						break;
					default:
						break;
				}
			}
			cnt++;
		}

		import_node (&ist, level, flags, 0, &cdata[level + cnt]);
	}

	fclose (file);

	if (node_getflag (node, F_temp))
		node = node_remove (node);	/* remove temporary node, if tree was empty */
	return (int) (node);
}

static void ascii_export_node (FILE * file, int level, int flags, char *data)
{

	indent (level, "\t");

	if (flags & F_todo) {		/* print the flags of the current node */
		if (flags & F_done)
			fprintf (file, "[X]");
		else
			fprintf (file, "[ ]");
	} else {
		if (data[0] == '[')		/* escape the first char */
			fprintf (file, "[]");
	}

	fprintf (file, "%s\n", data);
}

int export_ascii (char *params, void *data)
{
	Node *node = (Node *) data;
	char *filename = params;
	Node *tnode;
	int level, flags, startlevel;
	char *cdata;
	FILE *file;

	file_error[0] = 0;

	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		sprintf (file_error, "ascii export, unable to open \"%s\"", filename);
		return (int) node;
	}
	startlevel = nodes_left (node);

	tnode = node;

	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		cdata = node_getdata (tnode);
		ascii_export_node (file, level, flags, cdata);

		tnode = node_recurse (tnode);
	}

	if (file != stdout)
		fclose (file);
	return (int) node;
}
