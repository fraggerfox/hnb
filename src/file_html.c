/*
 * file_ascii.c -- html export filter for hnb
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
!cli cli_add_command ("export_html", export_html, "<filename>");
!clid int export_html ();
*/


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "cli.h"
#include "tree.h"
#include "file.h"

#define indent(count,char)	{int j;for(j=0;j<count;j++)fprintf(file,char);}

#define transform(a,b) case a:\
						{int j=0;const char * msg=b;\
							while(msg[j])\
								out[outpos++]=msg[j++];\
							out[outpos]=0;\
							inpos++;\
							break;\
						}\

static char *html_quote (const char *in)
{
	static char out[bufsize];
	int inpos = 0;
	int outpos = 0;

	out[0] = 0;
	while (in[inpos]) {
		switch (in[inpos]) {
				transform ('&', "&amp;");
				transform ('<', "&lt;");
				transform ('>', "&gt;");
				transform ('ø', "&oslash;");
				transform ('Ø', "&Oslash;");
				transform ('å', "&aring;");
				transform ('Å', "&Aring;");
				transform ('æ', "&aelig;");
				transform ('Æ', "&AElig;");
			default:
				out[outpos++] = in[inpos++];
				out[outpos] = 0;
				break;
		}
	}
	return (out);
}


int export_html (char *params, void *data)
{
	Node *node = (Node *) data;
	char *filename = params;
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *cdata;
	FILE *file;

	file_error[0] = 0;
	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		sprintf (file_error, "export html unable to open \"%s\"", filename);
		return (int) node;
	}

	startlevel = nodes_left (node);

	tnode = node;
	lastlevel = 0;
	fprintf (file, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n\
<HTML>\n\
<HEAD>\n\
	<TITLE>tree exported from hnb</TITLE>\n\
</HEAD>\n\
<BODY>\n\
<UL>\n");
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		cdata = node_getdata (tnode);

		if (level > lastlevel) {
			indent (level - 1, "\t");
			fprintf (file, "  <UL>\n");
		}

		if (level < lastlevel) {
			int level_diff = lastlevel - level;

			for (; level_diff; level_diff--) {
				indent (level + level_diff - 1, "\t");
				fprintf (file, "  </UL>\n");
			}
		}

		indent (level, "\t");
/*
 * FIXME use html_quote here?
 *
 */
		if (cdata[0] != 0) {
			fprintf (file, "<LI>%s%s</LI>\n",
					 (flags & F_todo ? (flags & F_done ? "[X] " : "[&nbsp] ")
					  : ""), html_quote (cdata));
		} else {
			fprintf (file, "<!-- empty line in input -->\n");
		}

		lastlevel = level;
		tnode = node_recurse (tnode);
	}
	level = 0;

	{
		int level_diff = lastlevel - level;

		for (; level_diff; level_diff--) {
			for (cnt = 0; cnt <= level + level_diff - 1; cnt++)
				fprintf (file, "\t");
			fprintf (file, "  </UL>\n");
		}
	}

	fprintf (file, "</UL>\n</BODY></HTML>");
	if (file != stdout)
		fclose (file);

	return (int) node;
}

#if 0
static int export_table (char *params, void *data)
{
	Node *node = (Node *) data;
	char *filename = params;
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *cdata;
	char tfilename[500];
	FILE *file;

	file_error[0] = 0;
	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		sprintf (file_error, "export html unable to open \"%s\"", filename);
		return (int) node;
	}

	startlevel = nodes_left (node);

	tnode = node;
	lastlevel = 0;
	fprintf (file, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n\
<HTML>\n\
<HEAD>\n\
	<TITLE>tree exported from hnb</TITLE>\n\
</HEAD>\n\
<BODY><table border='1'>\n");
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		cdata = node_getdata (tnode);


		switch (level) {
			case 0:
/*				fprintf(file,"<tr><td colspan='4'><h1>%s</h1></td></tr>\n",cdata);*/
				break;
			case 1:
				tfilename[0] = 0;
/*				fprintf(file,"<tr><td colspan='4'><h2>%s</h2></td></tr>\n",cdata);*/
				strcpy (tfilename, cdata);
				break;
			case 2:
				strcpy (&tfilename[3], cdata);
				break;
			case 3:
				fprintf (file,
						 "<td>%s.avi</td><td><img src='thumbs/%s.avi.jpg' width='80' height='60'></td><td valign='top' width='90%%'>%s</td></tr>\n",
						 tfilename, tfilename, cdata);
				break;
			default:
				break;
		}
		tnode = node_recurse (tnode);
	}
	fprintf (file, "\n</table></BODY></HTML>");
	if (file != stdout)
		fclose (file);

	return (int) node;
}

#endif
