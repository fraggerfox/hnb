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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
				transform ('\'', "&#39;");
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


static int export_html (char *params, void *data)
{
	Node *node = (Node *) data;
	char *filename = params;
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *cdata;
	FILE *file;

	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		cli_outfunf ("html export, unable to open \"%s\"", filename);
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
		cdata = fixnullstring(node_get (tnode, TEXT));

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

	cli_outfunf ("html export, saved output in \"%s\"", filename);
	return (int) node;
}

static void htmlcss_export_nodes (FILE * file, Node *node, int level)
{
	while (node) {
		char *data = fixnullstring(node_get (node, TEXT));

		fprintf (file, "\n");
		indent (level, "\t");
		fprintf (file, "<div>");	
		fprintf (file, "%s", html_quote (data));/* FIXME: use iconv to really create UTF-8 */

		if (node_right (node)) {
			htmlcss_export_nodes (file, node_right (node), level + 1);
			fprintf (file, "\n");
			indent (level, "\t");
			fprintf (file, "</div>");
		} else {
			fprintf (file, "</div>");
		}

		node = node_down (node);
	}
}


static int export_htmlcss (char *params, void *data)
{
	Node *node = (Node *) data;
	char *filename = params;
	FILE *file;

	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		cli_outfunf ("html export, unable to open \"%s\"", filename);
		return (int) node;
	}

	fprintf (file,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \
\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\
<html><head>\n\
<meta http-equiv=\"Content-type\" content=\"text/html; charset=UTF-8\" />\n\
<title>tree exported from hnb</title>\n\
<style type=\"text/css\" id=\"internalStyle\">\n\
div {\n\
	padding-top: 0.5em;\n\
	font-family: verdana, arial, helvetica, sans-serif; position:relative;\n\
	font-size:   10pt;\n\
	left:        2em;\n\
	padding-right: 2em;\n\
}\n\
</style>\n\
</head>\n\
<body xmlns=\"http://www.w3.org/1999/xhtml\">\n");

htmlcss_export_nodes(file,node,0);

fprintf (file, "\n</body></html>\n");
	if (file != stdout)
		fclose (file);

	cli_outfunf ("html export, saved output in \"%s\"", filename);
	return (int) node;
}

/*
!init_file_html();
*/
void init_file_html(){
	cli_add_command ("export_html", export_html, "<filename>");
	cli_add_command ("export_htmlcss", export_htmlcss, "<filename>");
}
