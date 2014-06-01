/*
 * file_opml.c -- generic xml import/export filters for hnb
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xml_tok.h"

#include "cli.h"
#include "tree.h"

#include "file.h"
#include "prefs.h"
#include "query.h"
#include "util_string.h"

#define indent(count,char)	{int j;for(j=0;j<count;j++)fprintf(file,char);}

/* *INDENT-OFF* */

static char *xmlquote[]={
	"<","&lt;",
	">","&gt;",
	"&","&amp;",
	"\"","&quot;",
	"'","&apos;",
	NULL
};

static char *xmlunquote[]={
	"&lt;","<",
	"&gt;",">",
	"&amp;","&",
	"&quot;","\"",
	"&apos;","'",
	NULL
};

/* *INDENT-ON* */

static void opml_export_nodes (FILE * file, Node *node, int level)
{
	while (node) {
		fprintf (file, "\n");
		indent (level, "\t");
		fprintf (file, "<outline");

		{Node_AttItem *att=node->attrib;
		 while(att){
		 	char *quoted=string_replace(att->data,xmlquote);
			fprintf (file, " %s=\"%s\"", att->name, quoted);
			free(quoted);
			att=att->next;
		 }		 
		}

		if (node_right (node)) {
			fprintf (file, ">");

			opml_export_nodes (file, node_right (node), level + 1);
			fprintf (file, "\n");
			indent (level, "\t");
			fprintf (file, "</outline>");
		} else {
			fprintf (file, " />");
		}

		node = node_down (node);
	}
}

static int export_opml (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename = argc>=2?argv[1]:"";
	FILE *file;

	if (!strcmp (filename, "*"))
		filename = query;
	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");

	if (!file) {
		cli_outfunf ("opml export, unable to open \"%s\"", filename);
		return (int) node;
	}

	fprintf (file, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?><!--pos:%s-->\n\
<!-- generated by hnb %s (http://hnb.sourceforge.net/) -->\n\
<opml version=\"1.0\">\n\
	<head>\n\
		<title>outline exported from hnb</title>\n\
		<dateCreated></dateCreated>\n\
		<dateModified></dateModified>\n\
		<ownerName></ownerName>\n\
		<ownerEmail></ownerEmail>\n\
		<expansionState></expansionState>\n\
		<vertScrollState></vertScrollState>\n\
		<windowTop>20</windowTop>\n\
		<windowLeft>20</windowLeft>\n\
		<windowBottom>200</windowBottom>\n\
		<windowRight>200</windowRight>\n\
		</head>\n\
	<body>\n", argc==3?argv[2]:"1",
			 VERSION);

	opml_export_nodes (file, node, 0);

	fprintf (file, "\n</body>\n</opml>\n");
	if (file != stdout)
		fclose (file);

	cli_outfunf ("opml export, wrote data to \"%s\"", filename);

	return (int) node;
}

static int import_opml (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename = argc==2?argv[1]:"";
	char *rdata;
	int type;
	int in_body = 0;
	int in_outlineelement = 0;
	int level = -1;
	xml_tok_state *s;
	import_state_t ist;

	Node *tempnode=NULL;
	FILE *file;

	if (!strcmp (filename, "*"))
		filename = query;
	file = fopen (filename, "r");
	if (!file) {
		cli_outfunf ("opml import, unable to open \"%s\"", filename);
		return (int) node;
	}
	s = xml_tok_init (file);
	init_import (&ist, node);

	while (((type = xml_tok_get (s, &rdata)) != t_eof) && (type != t_error)) {
		if (type == t_error) {
			cli_outfunf ("opml import error, parsing og '%s', line:%i %s", filename,
						 s->line_no, rdata);
			fclose (file);
			return (int) node;
		}
		if (in_body) {
			if (type == t_tag && !strcmp (rdata, "outline")) {
				level++;
				in_outlineelement = 1;
				tempnode=node_new();
				continue;
			}
			if (in_outlineelement && type == t_att){
				char *att_name=strdup(rdata);
				char *unquoted;
				if(xml_tok_get(s,&rdata)!=t_val){
					cli_outfun("import_opml: hmm I don't think this is according to OPML,..");
				};
				unquoted=string_replace(rdata,xmlunquote);

				node_set(tempnode,att_name,unquoted);
				free(unquoted);
				free(att_name);
				continue;
			}
			if ((type == t_endtag || type == t_closeemptytag)
				&& !strcmp (rdata, "outline")) {
				in_outlineelement = 0;
				import_node(&ist, level, tempnode);	/* will free tempnode */
				tempnode=NULL;
			}
			if ((type == t_closetag || type == t_closeemptytag)
				&& !strcmp (rdata, "outline")) {
				level--;
				continue;
			}
		} else {
			if (type == t_tag && !strcmp (rdata, "body"))
				in_body = 1;
		}
	}

	if (node_getflag (node, F_temp))
		node = node_remove (node);	/* remove temporary node, if tree was empty */

	cli_outfunf ("opml import - imported \"%s\" %i lines", filename, s->line_no);
	xml_tok_cleanup (s);
	return (int) node;
}


/*
!init_file_opml();
*/
void init_file_opml ()
{
	cli_add_command ("export_opml", export_opml, "<filename>");
	cli_add_command ("import_opml", import_opml, "<filename>");
}
