/*
 * file.c -- file import and export functions for hnb's tree
 *
 * Copyright (C) 2001,2001 Øyvind Kolås <pippin@users.sourceforge.net>
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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tree.h"

#define bufsize 4096

static Node *npos;
	/* pointer within to keep track of where we are, whilst importing */
static int startlevel;
	/* says how deep in the tree we started importing */

void import_node (int level, int flags, unsigned char priority, char *data){
	int node_level;
	
	level += startlevel;
	
	while ((node_level = nodes_left (npos)) > level)
		npos = node_left (npos);
	if (node_level == level)
		npos = node_insert_down (npos);
	if (node_level < level)
		npos = node_insert_right (npos);
	node_setflags (npos, flags);
	node_setpriority (npos, priority);	
	node_setdata (npos, data);
}

static void ascii_export_node (FILE * file, int level, int flags, char *data){
	#define indent(a)	{int j;for(j=0;j<a;j++)fprintf(file,"\t");}
	
	indent(level);
	
	if (flags & F_todo) {		/* print the flags of the current node */
		if (flags & F_done)
			fprintf (file, "[X]");
		else
			fprintf (file, "[ ]");
	} else {				
		if (data[0] == '[')	/* escape the first char */
			fprintf (file, "[]");
	}
	
	fprintf (file, "%s\n", data);
}

Node *ascii_import (Node *node, char *filename){
	int level, flags, cnt;
	char data[bufsize];
	FILE *file;
	
	file = fopen (filename, "r");
	if (file == NULL)
		return (node);
	
	npos = node;
	startlevel = nodes_left (node);
	
	while (fgets (data, bufsize, file) != NULL) {
		flags = level = cnt = 0;
	
					/*strip newlines and carrier return  */
	while(data[strlen(data)-1]==13 || data[strlen(data)-1]==10)	
		data[strlen (data) - 1] = 0;
		
	while (data[level] == '\t')	/* find the level of this node */
		level++;
	
	if (data[level] == '[') {	/* read the flags */
		while (data[level + cnt] != ']') {
			cnt++;
				switch (data[level + cnt]) {
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
	
	import_node (level, flags, 0, &data[level + cnt]);
	}
	
	fclose (file);
	
	if (node_getflag (node, F_temp))
		node = node_remove (node);	/* remove temporary node, if tree was empty */
	return (node);
}

void ascii_export (Node *node, char *filename){
	Node *tnode;
	int level, flags, startlevel;
	char *data;
	FILE *file;
	
	file = fopen (filename, "w");
	startlevel = nodes_left (node);
	
	tnode = node;
	
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		data = node_getdata (tnode);
		ascii_export_node (file, level, flags, data);
	
		tnode = node_recurse (tnode);
	}
	
	fclose (file);
}

char *xml_quote(char *in){
	static char out[bufsize+30]; /* for added tags'n'tabs */
	int inpos=0;
	int outpos=0;	

	out[0]=0;
	strcpy(&out[outpos],"<data>");outpos+=6;
	
	while(in[inpos]){
		switch(in[inpos]){
			case '&':	strcpy(&out[outpos],"&amp;");outpos+=5;inpos++;break;
			case '<':	strcpy(&out[outpos],"&lt;");outpos+=4;inpos++;break;
			case '>':	strcpy(&out[outpos],"&gt;");outpos+=4;inpos++;break;
			case '\'':	strcpy(&out[outpos],"&apos;");outpos+=6;inpos++;break;
			case '"':	strcpy(&out[outpos],"&quot;");outpos+=6;inpos++;break;
			default:
				out[outpos++]=in[inpos++];
				out[outpos]=0;
				break;
		}
	}
	
	strcpy(&out[outpos],"</data>");outpos+=7;
		
	return(out);
}

static void xml_export_nodes (FILE * file, Node *node, int level){
	int flags;
	char *data;
	unsigned char priority;
	
	#define indent(a)	{int j;for(j=0;j<a;j++)fprintf(file,"\t");}
	
	while(node){
		flags = node_getflags (node);
		data = node_getdata (node);
		priority = node_getpriority(node);
		
		fprintf(file,"\n");
		indent(level);
		fprintf(file,"<node");  /* the start tag with attributes if any */
		if(flags&F_todo){
			fprintf(file," done=");
			if(flags&F_done){
				fprintf(file,"\"yes\"");
			} else {
				fprintf(file,"\"no\"");
			}
		if(priority!=0)
			fprintf(file," priority=\"%i\"",priority);		
		}
		fprintf(file,">");

		fprintf(file,"%s",xml_quote(data));
	
		if(node_right(node)){
			xml_export_nodes(file,node_right(node),level+1);
			fprintf(file,"\n");	
			indent(level);
			fprintf(file,"</node>");	
		} else {
			fprintf(file,"</node>");			
		}

		node=node_down(node);
	}
}

void xml_export (Node *node, char *filename){
	FILE *file;
	
	file = fopen (filename, "w");

	fprintf(file,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n\
\n\
<!-- this is the native format of hnb (http://hnb.sourceforge.net/) -->\n\
<!-- generated by hnb %s -->\n\
\n\
<!DOCTYPE tree[\n\
	<!ELEMENT tree (node*)>\n\
	<!ELEMENT data (#PCDATA)>\n\
	<!ELEMENT node (data?,node*)>\n\
	<!ATTLIST node done (yes|no) #IMPLIED priority (1|2|3|4|5|6|7|8|9) #IMPLIED>\n\
	<!-- maximum line length: 4096 -->\n\
]>\n\
\n\
<tree>",VERSION);	
	
	xml_export_nodes (file, node,0);
	
	fprintf(file,"\n</tree>\n");		
	fclose (file);
}

/* returns 1 if the first couple of lines of file contains 'xml' */
int xml_check(char *filename){
	FILE *file;
	char buf[bufsize];
	int j;
	
	file = fopen (filename, "r");
	if (file == NULL)
		return -1;

	for(j=0;j<2;j++){
		if (fgets (buf, bufsize, file) == NULL){
			fclose(file);
			return 0;
		}
		if(strstr(buf,"xml")!=0){
			fclose(file);
			return 1;
		}
	}
	fclose(file);
	return 0;
}

			/*	single pass xml import filter for hnb's DTD */
Node *xml_import (Node *node, char *filename){
	int level, flags=0;	unsigned char priority=0;	
	char data[bufsize]; /* data to store in tree */
	int dpos=0;		/* how much data we've got yet */
	
	char buf[bufsize+30]; /* line buffer */
	int cnt=0;				/* position in line buffer */	

	int in_data=0;	/* for remembering where we are */
	int in_dtd=0;
	int in_comment=0;
	
	FILE *file;
	
	file = fopen (filename, "r");
	if (file == NULL)
		return (node);
	
	npos = node;
	startlevel = nodes_left (node);
	level=0;
	
	while (fgets (buf, bufsize, file) != NULL) {
		cnt=0;

		while(buf[cnt]){
			if(in_dtd){
				if(in_dtd==1){
					if(buf[cnt]==']')in_dtd=2;
				} else if(buf[cnt]=='>')in_dtd=0;
			} else if(in_comment) {
				if(!strncmp(&buf[cnt],	"-->",3)){
					in_comment--;
					cnt+=2;
				}				
			} else switch(buf[cnt]){
				case '<':
					if(!strncmp(&buf[cnt+1],			"?xml",4))	{ /* xml version */
						while( (buf[++cnt] != '>'));
					} else if(!strncmp(&buf[cnt+1],		"!DO",3))	{ /* DTD starts */
							in_dtd=1;
					} else if(!strncmp(&buf[cnt+1],		"!--",3))	{ /* comment start */
							in_comment++;
					} else if(!strncmp(&buf[cnt+1],		"tree",4))	{ /* starting tree */
						while (buf[++cnt] != '>');
					} else if(!strncmp(&buf[cnt+1],		"/tree",5))	{ /* closing tree */
						while (buf[++cnt] != '>');
					} else if(!strncmp(&buf[cnt+1],		"data",4))	{ /* starting data */
						data[0]=0;
						dpos=0;
						in_data=1;
						while (buf[++cnt] != '>');
					} else if(!strncmp(&buf[cnt+1],		"/data",5)) { /* closing data */
						if(data[dpos-1]==' ')
							data[dpos-1]=0;	/* removing trailing space */	
						import_node (level-1, flags, priority, data);
						in_data=0;
						while (buf[++cnt] != '>');
					} else if(!strncmp(&buf[cnt+1],		"node",4))	{ /* starting node */
						level++;						/* level up */
						flags = priority = 0;			/* reset variables */
						while (buf[++cnt] != '>') { 	/* fill variables with attributes */ 
							if(!strncmp(&buf[cnt+1],"done=",5)) { 		
								if(buf[cnt+7]=='y')
									flags = flags + F_todo + F_done;
								else
									flags = flags + F_todo;
							} else if(!strncmp(&buf[cnt+1],"priority=",9)) { 
								priority=(unsigned char)buf[cnt+11]-48;
							}
						}
					} else if(!strncmp(&buf[cnt+1],		"/node",5))	{ /* closing node */
						level--;			/* level down */
						while (buf[++cnt] != '>');
					}
					break;
				case '&':{int epos=dpos+1;	/* entity */
					data[dpos++]=buf[cnt];
					data[dpos]=0;
					while(buf[++cnt]!=';')
						data[epos++]=buf[cnt];
						data[epos]=0;
					}
					if(!strcmp(&data[dpos],"amp")){data[dpos-1]='&';data[dpos]=0;}
					if(!strcmp(&data[dpos],"lt")){data[dpos-1]='<';data[dpos]=0;}
					if(!strcmp(&data[dpos],"gt")){data[dpos-1]='>';data[dpos]=0;}
					if(!strcmp(&data[dpos],"apos")){data[dpos-1]='\'';data[dpos]=0;}
					if(!strcmp(&data[dpos],"quot")){data[dpos-1]='"';data[dpos]=0;}
					break;
				case ' ':					/*white space*/
				case '\t':
				case 13:
				case 10: 
					if(in_data && data[0] && data[dpos-1]!=' '){
						data[dpos++]=' ';	/*only add if last char wasn't */
						data[dpos]=0;
					}
					break;
				default:
					if(in_data){
						data[dpos++]=buf[cnt];
						data[dpos]=0;
					}
					break;
			}
			cnt++;
		}
	}
		fclose (file);
	
	if (node_getflag (node, F_temp))	/* remove temporary node, if tree was empty */
		node = node_remove (node);
	return (node);
}


char *html_quote(char *in){
	static char out[bufsize];
	int inpos=0;
	int outpos=0;

	out[0]=0;
	while(in[inpos]){
		switch(in[inpos]){
			case '&':	strcpy(&out[outpos],"&amp;");outpos+=5;inpos++;break;
			case '<':	strcpy(&out[outpos],"&lt;");outpos+=4;inpos++;break;
			case '>':	strcpy(&out[outpos],"&gt;");outpos+=4;inpos++;break;
			default:
				out[outpos++]=in[inpos++];
				out[outpos]=0;
				break;
		}
	}
	return(out);
}

void html_export (Node *node, char *filename){
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *data;
	FILE *file;
	
	file = fopen (filename, "w");
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
		data = node_getdata (tnode);
		
	if (level > lastlevel) {
		for (cnt = 0; cnt <= level - 1; cnt++) {
			fprintf (file, "\t");
			};
			fprintf (file, "  <UL>\n");
		}
	
	if (level < lastlevel) {
		int level_diff = lastlevel - level;
	
	for (; level_diff; level_diff--) {
		for (cnt = 0; cnt <= level + level_diff - 1; cnt++)
			fprintf (file, "\t");
				fprintf (file, "  </UL>\n");
			}
		}
	
	for (cnt = 0; cnt <= level; cnt++)
		fprintf (file, "\t");
	
	if (data[0] != 0) {
		fprintf (file, "<LI>%s%s\n",
			 (flags & F_todo ? (flags & F_done ? "[X] " : "[&nbsp] ")
				  : ""), data);
		} else {
			fprintf (file, "<BR><BR>\n");
		}
	
	lastlevel = level;
		tnode = node_recurse (tnode);
	};
	level = 0;
	
	{
		int level_diff = lastlevel - level;
	
	for (; level_diff; level_diff--) {
		for (cnt = 0; cnt <= level + level_diff - 1; cnt++)
			fprintf (file, "\t");
			fprintf (file, "  </UL>\n");
		};
	}
	
	fprintf (file, "</UL>\n</BODY></HTML>");
	fclose (file);
}

char *help_quote(char *in){
	static char out[bufsize+10];
	int inpos=0;
	int outpos=0;

	out[0]=0;

	while(in[inpos]){
		switch(in[inpos]){
			case '\\':	strcpy(&out[outpos],"\\\\");outpos+=2;inpos++;break;
			case '"':	strcpy(&out[outpos],"\"");outpos+=2;inpos++;break;
			default:
				out[outpos++]=in[inpos++];
				out[outpos]=0;
				break;
		}
	}
	
		
	return(out);
}

void help_export (Node *node, char *filename){
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *data;
	FILE *file;
	
	file = fopen (filename, "w");
	startlevel = nodes_left (node);
	
	tnode = node;
	lastlevel = 0;
	fprintf (file, "#define i(a,b,c) import_node(a,c,0,b)\n\n");
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		data = node_getdata (tnode);
	
	for (cnt = 0; cnt < level; cnt++)
		fprintf (file, "\t");
	
	fprintf (file, "i(%i,\"%s\",%i);\n", level, help_quote(data), flags);
	
	lastlevel = level;
		tnode = node_recurse (tnode);
	};
	level = 0;
	
	fclose (file);
}

Node *help_import (Node *node){
	npos = node;
	startlevel = nodes_left (node);
#include "tutorial.inc"

if (node_getflag (node, F_temp))
	node = node_remove (node);

return (node);
}


void gxml_export (Node *node, char *filename){
}
Node *gxml_import (Node *node, char *filename){
	return node;
}
