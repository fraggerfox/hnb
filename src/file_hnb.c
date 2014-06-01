/*
 * file_ascii.c -- hnb XML-dtd import and export filters for hnb
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


/*
!cli cli_add_command ("export_hnb", export_hnb, "<filename>");
!cli cli_add_command ("import_hnb", import_hnb, "<filename>");

!clid int import_hnb ();
!clid int export_hnb ();
*/


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xml_tok.h"

#include "cli.h"
#include "tree.h"

#include "file.h"
#include "prefs.h"

#define indent(count,char)	{int j;for(j=0;j<count;j++)fprintf(file,char);}

#define transform(a,b) case a:\
						{int j=0;const char * msg=b;\
							while(msg[j])\
								out[outpos++]=msg[j++];\
							out[outpos]=0;\
							inpos++;\
							break;\
						}\

/* converts special chars into entities */
static char *xml_quote (const char *in)
{
	static char out[bufsize + 30];	/* for added tags'n'tabs */
	int inpos = 0;
	int outpos = 0;

	out[0] = 0;

	while (in[inpos]) {
		switch (in[inpos]) {
				transform ('&', "&amp;");
				transform ('<', "&lt;");
				transform ('>', "&gt;");
				transform ('\'', "&apos;");
				transform ('"', "&quot;");
			default:
				out[outpos++] = in[inpos++];
				out[outpos] = 0;
				break;
		}
	}
	return (out);
}



static void hnb_export_nodes (FILE * file, Node *node, int level)
{
	while (node) {
		int flags = node_getflags (node);
		char *data = node_getdata (node);
		int priority = node_getpriority (node);
		int percent_done = node_getpercent_done(node);
		int size=node_getsize(node);

		fprintf (file, "\n");
		indent (level, "\t");
		fprintf (file, "<node");	/* the start tag with attributes if any */
		if (flags & F_todo) {
			fprintf (file, " done=");
			if (flags & F_done) {
				fprintf (file, "\"yes\"");
			} else {
				fprintf (file, "\"no\"");
			}
		}
		if (priority != 0)
			fprintf (file, " priority=\"%i\"", priority);
		if (percent_done != -1)
			fprintf (file, " percent_done=\"%i\"", percent_done);
		if (size != -1)
			fprintf (file, " size=\"%i\"", size);


		fprintf (file, ">");

		fprintf (file, "<data>%s</data>", xml_quote (data));

		if (node_right (node)) {
			hnb_export_nodes (file, node_right (node), level + 1);
			fprintf (file, "\n");
			indent (level, "\t");
			fprintf (file, "</node>");
		} else {
			fprintf (file, "</node>");
		}

		node = node_down (node);
	}
}

int export_hnb (char *params, void *data)
{
	Node *node = (Node *) data;
	char *filename = params;
	FILE *file;

	while (*params && (*params != ' '))
		params++;
	if (*params == ' ') {
		*params = 0;
		params++;
	}

	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");

	if (!file) {
		cli_outfunf ("hnb export, unable to open \"%s\"", filename);
		return (int) node;
	}

	fprintf (file, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?><!--pos:%s-->\n\
<!-- generated by hnb %s (http://hnb.sourceforge.net) -->\n\
\n\
<!DOCTYPE tree[\n\
	<!ELEMENT tree (node*)>\n\
	<!ELEMENT data (#PCDATA)> <!-- (max 4096 bytes long) -->\n\
	<!ELEMENT node (data?,node*)>\n\
	<!ATTLIST node done (yes|no) #IMPLIED priority (1|2|3|4|5|6|7|8|9) #IMPLIED> ]>\n\
\n\
<tree>", params,
			 VERSION);

	hnb_export_nodes (file, node, 0);

	fprintf (file, "\n</tree>\n");
	if (file != stdout)
		fclose (file);

	cli_outfunf ("hnb export, wrote data to \"%s\"", filename);

	return (int) node;
}


int import_hnb (char *params, void *data)
{
	Node *node = (Node *) data;
	char *filename = params;
	char *rdata;
	int type;
	int in_tree = 0;
	int level = -1;
	int priority = 0;
	char nodedata[4096];
	int nodedatapos = 0;
	int percent_done = -1;
	int size = -1;
	int flags = 0;
	int in_data = 0;
	xml_tok_state *s;
	import_state_t ist;

	FILE *file;


	file = fopen (filename, "r");
	if (!file) {
		cli_outfunf ("hnb import, unable to open \"%s\"", filename);
		return (int) node;
	}
	s = xml_tok_init (file);
	init_import (&ist, node);

	while (((type = xml_tok_get (s, &rdata)) != t_eof) && (type != t_error)) {
		if (type == t_error) {
			cli_outfunf ("hnb import error, parsing og '%s', %s",filename, rdata);
			fclose (file);
			return (int) node;
		}
		if (in_tree) {
			if (type == t_tag && !strcmp (rdata, "node")) {
				priority = 0;
				flags = 0;
				level++;
			}
			if (type == t_att && !strcmp (rdata, "done")) {
				xml_tok_get (s, &rdata);
				if (!strcmp ("no", rdata)) {
					flags = F_todo;
				} else if (!strcmp ("yes", rdata)) {
					flags = F_todo + F_done;
				}
				continue;
			}
			if (type == t_att && !strcmp (rdata, "percent_done")) {
				xml_tok_get (s, &rdata);
				percent_done = atoi (rdata);
				continue;
			}
			if (type == t_att && !strcmp (rdata, "priority")) {
				xml_tok_get (s, &rdata);
				priority = atoi (rdata);
				continue;
			}
			if (type == t_att && !strcmp (rdata, "size")) {
				xml_tok_get (s, &rdata);
				size = atoi (rdata);
				continue;
			}
			if (type == t_tag && !strcmp (rdata, "data")) {
				nodedatapos = 0;
				nodedata[nodedatapos] = 0;
				in_data = 1;
			}
			if (in_data) {
				switch (type) {
					case t_whitespace:
						if (nodedatapos)
							nodedata[nodedatapos++] = ' ';
						nodedata[nodedatapos] = 0;
						break;
					case t_entity:
						if (!strcmp (rdata, "amp")) {
							nodedata[nodedatapos++] = '&';
							nodedata[nodedatapos] = 0;
						} else if (!strcmp (rdata, "gt")) {
							nodedata[nodedatapos++] = '>';
							nodedata[nodedatapos] = 0;
						} else if (!strcmp (rdata, "lt")) {
							nodedata[nodedatapos++] = '<';
							nodedata[nodedatapos] = 0;
						} else if (!strcmp (rdata, "quot")) {
							nodedata[nodedatapos++] = '"';
							nodedata[nodedatapos] = 0;
						} else if (!strcmp (rdata, "apos")) {
							nodedata[nodedatapos++] = '\'';
							nodedata[nodedatapos] = 0;
						} else {
							/* unhandled entity,.. might as well do a standard html and xml transform?,.. or ? */
						}
						break;
					case t_word:
						strcpy (&nodedata[nodedatapos], rdata);
						nodedatapos = strlen (nodedata);
						break;
					default:
						break;
				}
			}
			if (type == t_closetag && !strcmp (rdata, "data")) {
				Node *imported=import_node (&ist, level, flags, priority, nodedata);
				node_setpercent_done(imported,percent_done);
				node_setsize(imported,size);
				percent_done=-1;	
				size=-1;
				in_data = 0;
			}
			if (type == t_closetag && !strcmp (rdata, "node")) {
				level--;
			}
			if (type == t_closetag && !strcmp (rdata, "tree"))
				in_tree = 0;
		} else {
			if (type == t_tag && !strcmp (rdata, "tree"))
				in_tree = 1;
		}
	}

	if (node_getflag (node, F_temp))
		node = node_remove (node);	/* remove temporary node, if tree was empty */

	cli_outfunf ("hnb import - imported \"%s\"", filename);

	return (int) node;
}
