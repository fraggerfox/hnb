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
	"<" , "&lt;",
	">" , "&gt;",
	"&" , "&amp;",
	"\"", "&quot;",
	"'" , "&apos;",
	NULL
};

static char *xmlunquote[]={
	"&lt;"   , "<",
	"&gt;"   , ">",
	"&amp;"  , "&",
	"&quot;" , "\"",
	"&apos;" , "'",
	NULL
};

/* *INDENT-ON* */


static void hnb_export_nodes (FILE * file, Node *node, int level)
{
	while (node) {
		char *data = fixnullstring (node_get (node, TEXT));

		fprintf (file, "\n");
		indent (level, "\t");
		fprintf (file, "<node");	/* the start tag with attributes if any */

		{Node_AttItem *att=node->attrib;
		 while(att){
		 	char *quoted=string_replace(att->data,xmlquote);
			fprintf (file, " %s=\"%s\"", att->name, quoted);
			free(quoted);
			att=att->next;
		 }		 
		}


		fprintf (file, ">");

		{
			char *quoted=string_replace(data,xmlquote);
			fprintf (file, "<data>%s</data>", quoted);
			free(quoted);
		}

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

static int export_hnb (int argc, char **argv, void *data)
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
	<!ATTLIST node done (yes|no) #IMPLIED> ]>\n\
\n\
<tree>", (argc==3)?argv[2]:"",
			 VERSION);

	hnb_export_nodes (file, node, 0);

	fprintf (file, "\n</tree>\n");
	if (file != stdout)
		fclose (file);

	cli_outfunf ("hnb export, wrote data to \"%s\"", filename);

	return (int) node;
}


static int import_hnb (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename = argc==2?argv[1]:"";
	char *rdata;
	int type;
	int in_tree = 0;
	int level = -1;
	char nodedata[4096];
	int nodedatapos = 0;
	int in_data = 0;
	int in_nodetag = 0;
	xml_tok_state *s;
	import_state_t ist;

	Node *tempnode=NULL;


	FILE *file;

	if (!strcmp (filename, "*"))
		filename = query;
	file = fopen (filename, "r");
	if (!file) {
		cli_outfunf ("hnb import, unable to open \"%s\"", filename);
		return (int) node;
	}
	s = xml_tok_init (file);
	init_import (&ist, node);

	while (((type = xml_tok_get (s, &rdata)) != t_eof) && (type != t_error)) {
		if (type == t_error) {
			cli_outfunf ("hnb import error, parsing og '%s' line:%i, %s", filename,
						 s->line_no, rdata);
			fclose (file);
			return (int) node;
		}
		if (in_tree) {
			if (type == t_tag && !strcmp (rdata, "node")) {
				level++;
				tempnode=node_new();
				in_nodetag=1;
				continue;
			}
			if(in_nodetag && type == t_att){
				char *att_name=strdup(rdata);
				if(xml_tok_get(s,&rdata)!=t_val){
					cli_outfun("import_hnb,.. hmpf....");
				};
				{char *unquoted=string_replace(rdata,xmlunquote);
				node_set(tempnode,att_name,unquoted);
				free(unquoted);
				}

				if(!strcmp(att_name,"done")){ /* to make older files conform */
					node_set(tempnode,"type","todo");
				}

				free(att_name);
				continue;
			}				
			if ( (type == t_endtag || type == t_closeemptytag) && !strcmp(rdata,"node")){
				in_nodetag=0;
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
						sprintf (&nodedata[strlen (nodedata)], "&%s;", rdata);
						nodedatapos+=strlen(rdata)+2;
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
				char *unquoted=string_replace(nodedata,xmlunquote);
				node_set(tempnode,TEXT,unquoted);
				free(unquoted);
				import_node (&ist, level, tempnode);
				tempnode=NULL;
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

	cli_outfunf ("hnb import - imported \"%s\" %i lines", filename, s->line_no);
	xml_tok_cleanup (s);
	return (int) node;
}

/*
!init_file_hnb();
*/
void init_file_hnb ()
{
	cli_add_command ("export_hnb", export_hnb, "<filename>");
	cli_add_command ("import_hnb", import_hnb, "<filename>");
}
