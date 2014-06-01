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
#include <string.h>
#include <unistd.h>
#include "tree.h"
#include "prefs.h"
#include "file.h"

#ifdef USE_LIBXML
#include <libxml/tree.h>
#include <libxml/parser.h>
#define XMLCHAR(n) (const xmlChar *)(const char *)n
static xmlDocPtr xmldoc = NULL;
#endif

#define bufsize 4096

/*prints a b chars to file */
#define indent(a,b)	{int j;for(j=0;j<a;j++)fprintf(file,b);}

void init_import(import_state_t *is, Node *node)
{
	is->npos = node;
	is->startlevel = nodes_left (node);
}

void import_node (import_state_t *is, int level, int flags, 
			int priority, char *data){
	int node_level;
	
	level += is->startlevel;

	while ((node_level = nodes_left (is->npos)) > level)
		is->npos = node_left (is->npos);
	if (node_level == level)
		is->npos = node_insert_down (is->npos);
	if (node_level < level)
		is->npos = node_insert_right (is->npos);
	node_setflags (is->npos, flags);
	node_setpriority (is->npos, priority);	
	node_setdata (is->npos, data);

/*	node_update_parents_todo(is->npos); commented out due to major slowdown
when importing */ 
}

Node *ascii_import (Node *node, char *filename){
	int level, flags, cnt;
	import_state_t ist;
	char data[bufsize];
	FILE *file;
	
	file = fopen (filename, "r");
	if (file == NULL)
		return (node);
	
	init_import(&ist, node);
	
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
	
	import_node (&ist, level, flags, 0, &data[level + cnt]);
	}
	
	fclose (file);
	
	if (node_getflag (node, F_temp))
		node = node_remove (node);	/* remove temporary node, if tree was empty */
	return (node);
}

static void ascii_export_node (FILE * file, int level, int flags, char *data){

	indent(level,"\t");
	
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

void ascii_export (Node *node, char *filename){
	Node *tnode;
	int level, flags, startlevel;
	char *data;
	FILE *file;
	
	if(!strcmp(filename,"-"))file=stdout;
	else file = fopen (filename, "w");

	startlevel = nodes_left (node);
	
	tnode = node;
	
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		data = node_getdata (tnode);
		ascii_export_node (file, level, flags, data);
	
		tnode = node_recurse (tnode);
	}
	
	if(file!=stdout)fclose (file);
}

#define transform(a,b) case a:\
						{int j=0;char * msg=b;\
							while(msg[j])\
								out[outpos++]=msg[j++];\
							out[outpos]=0;\
							inpos++;\
							break;\
						}\


/* converts special chars into entities */
char *xml_quote(const char *in){
	static char out[bufsize+30]; /* for added tags'n'tabs */
	int inpos=0;
	int outpos=0;
	out[0]=0;
						
	while(in[inpos]){
		switch(in[inpos]){
			transform('&',"&amp;");
			transform('<',"&lt;");
			transform('>',"&gt;");			
			transform('\'',"&apos;");
			transform('"',"&quot;");
			default:
				out[outpos++]=in[inpos++];
				out[outpos]=0;
				break;
		}
	}	
	return(out);
}

char *help_quote(char *in){
	static char out[bufsize+10];
	int inpos=0;
	int outpos=0;

	out[0]=0;

	while(in[inpos]){
		switch(in[inpos]){
			transform('\\',"\\\\");
			transform('"',"\\\"");
			default:
				out[outpos++]=in[inpos++];
				out[outpos]=0;
				break;
		}
	}
	
		
	return(out);
}

char *html_quote(char *in){
	static char out[bufsize];
	int inpos=0;
	int outpos=0;

	out[0]=0;
	while(in[inpos]){
		switch(in[inpos]){
			transform('&',"&amp;");
			transform('<',"&lt;");
			transform('>',"&gt;");
			transform('ø',"&oslash;");
			transform('Ø',"&Oslash;");
			transform('å',"&aring;");
			transform('Å',"&Aring;");
			transform('æ',"&aelig;");
			transform('Æ',"&AElig;");
			default:
				out[outpos++]=in[inpos++];
				out[outpos]=0;
				break;
		}
	}
	return(out);
}

static void xml_export_nodes (FILE * file, Node *node, int level){
	int flags;
	char *data;
	unsigned char priority;
	
	while(node){
		flags = node_getflags (node);
		data = node_getdata (node);
		priority = node_getpriority(node);
		
		fprintf(file,"\n");
		indent(level,"\t");
		fprintf(file,"<node");  /* the start tag with attributes if any */
		if(flags&F_todo){
			fprintf(file," done=");
			if(flags&F_done){
				fprintf(file,"\"yes\"");
			} else {
				fprintf(file,"\"no\"");
			}
		}
		if(priority!=0)
			fprintf(file," priority=\"%i\"",priority);		
		
		fprintf(file,">");

		fprintf(file,"<data>%s</data>",xml_quote(data));
	
		if(node_right(node)){
			xml_export_nodes(file,node_right(node),level+1);
			fprintf(file,"\n");	
			indent(level,"\t");
			fprintf(file,"</node>");	
		} else {
			fprintf(file,"</node>");			
		}

		node=node_down(node);
	}
}

void hnb_export (Node *node, char *filename){
	FILE *file;
	
	if(!strcmp(filename,"-"))file=stdout;
	else file = fopen (filename, "w");

	fprintf(file,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n\
<!-- generated by hnb %s (http://hnb.sourceforge.net) -->\n\
\n\
<!DOCTYPE tree[\n\
	<!ELEMENT tree (node*)>\n\
	<!ELEMENT data (#PCDATA)> <!-- (max 4096 bytes long) -->\n\
	<!ELEMENT node (data?,node*)>\n\
	<!ATTLIST node done (yes|no) #IMPLIED priority (1|2|3|4|5|6|7|8|9) #IMPLIED> ]>\n\
\n\
<tree>",VERSION);	
	
	xml_export_nodes (file, node,0);
	
	fprintf(file,"\n</tree>\n");		
	if(file!=stdout)fclose (file);	
	
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

/*returns 1 if file exists*/
int file_check(char *filename)
{
	FILE *file;
	file=fopen(filename,"r");
	if(file==NULL)
		return 0;
	fclose(file);
	return 1;
}

void html_export (Node *node, char *filename){
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *data;
	FILE *file;
	
	if(!strcmp(filename,"-"))file=stdout;
	else file = fopen (filename, "w");

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
		indent(level-1,"\t");
		fprintf (file, "  <UL>\n");
	}
	
	if (level < lastlevel) {
		int level_diff = lastlevel - level;
	
		for (; level_diff; level_diff--) {
			indent(level+level_diff-1,"\t");
			fprintf (file, "  </UL>\n");
		}
	}
	
	indent(level,"\t");
	
	if (data[0] != 0) {
		fprintf (file, "<LI>%s%s</LI>\n",
			 (flags & F_todo ? (flags & F_done ? "[X] " : "[&nbsp] ")
				  : ""), data);
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
	if(file!=stdout)fclose (file);
	
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
	fprintf (file, "#define i(a,b,c) import_node(&ist,a,c,0,b)\n\n");
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		data = node_getdata (tnode);
	
	for (cnt = 0; cnt < level; cnt++)
		fprintf (file, "\t");
	
	fprintf (file, "i(%i,\"%s\",%i);\n", level, help_quote(data), flags);
	
	lastlevel = level;
		tnode = node_recurse (tnode);
	}
	level = 0;
	
	fclose (file);
}

Node *help_import (Node *node){
	import_state_t ist;

	init_import(&ist, node);
	/*npos = node;*/
	/*startlevel = nodes_left (node);*/
#include "tutorial.inc"

if (node_getflag (node, F_temp))
	node = node_remove (node);

return (node);
}

/* returns the first occurence of one of the needles, or 0 (termination)
   if not found, return 0*/
int findchar(char *haystack,char *needles){
	int j=0;int k;
	while(haystack[j]){
		for(k=0;k<strlen(needles)+1;k++)
		if(haystack[j]==needles[k])
			return j;
		j++;
	}
	return 0;
}

static void gxml_export_nodes (FILE * file, Node *node, int level){
	char tag[bufsize];
	int flags;
	char *data;

	static int no_quote=0;

	#define indent(a,b)	{int j;for(j=0;j<a;j++)fprintf(file,b);}

	while(node){
		int data_start=0;
		tag[0]=0;
		flags = node_getflags (node);
		data = node_getdata (node);

		indent(level,"  ");

		if(data[0]=='<'){  /* calculate start tag, if any */      
			strcpy(tag,data);
			data_start=findchar(tag,">")+1;
			tag[data_start]=0;
			if(data[1]=='!'||data[1]=='?'){
				no_quote++;
			}	
		}

		if(no_quote)
			fprintf(file,"%s%s",tag,&data[data_start]);
		else
			fprintf(file,"%s%s",tag,xml_quote(&data[data_start]));
		
		if(data[0]=='<'){ /* calculate end tag */
			strcpy(tag,data);
			tag[findchar(tag," \t>")+1]=0;
			tag[findchar(tag," \t>")]='>';
			tag[0]='/';
		}

		if(node_right(node)){
			fprintf(file,"\n");	
			gxml_export_nodes(file,node_right(node),level+1);
			indent(level,"  ");
			if(data[0]=='<'){
				if(data[1]=='!' && data[2]=='-'){
					fprintf(file," -->\n");
				} else if(tag[1]!='?' && tag[1]!='!') {
					fprintf(file,"<%s\n",tag);
				} else {
					fprintf(file,"\n");
				}
			}
		} else {
			if(data[0]=='<' && data[strlen(data)-2]!='/'){ 
				if(data[1]=='!' && data[2]=='-'){
					fprintf(file," -->\n");
				}else if(tag[1]!='?' && tag[1]!='!'){
					fprintf(file,"<%s\n",tag);
				}else{
					fprintf(file,"\n");
				}
			} else
			fprintf(file,"\n");
		}
		if(data[0]=='<'&&(data[1]=='!'||data[1]=='?')){
			no_quote--;
		}	
		
		node=node_down(node);
	}
}

void xml_export (Node *node, char *filename){
	FILE *file;
	
	if(!strcmp(filename,"-"))file=stdout;
	else file = fopen (filename, "w");
	
	gxml_export_nodes (file, node,0);
	
	if(file!=stdout)fclose (file);
}

/* joins up tags with data if there is data as the first child
   of the tag.*/
Node *gxml_cuddle_nodes(Node *node){

	Node *tnode;
	char *tdata;
	char data[bufsize];
	
	tnode=node;

	while(tnode){
		if(node_right(tnode)){
			tdata=node_getdata(node_right(tnode));
			if(tdata[0]!='<'){	/* not a child tag */
				strcpy(data,node_getdata(tnode));
				strcat(data," ");
				strcat(data,tdata);
				node_setdata(tnode,data);
				node_remove(node_right(tnode));
			}
		}
		tnode=node_recurse(tnode);
	}

	return(node);

}

/*	removes double whitspaces from data, and translates enities into chars
*/
char *xml_unquote(char *in,int generic){
	static char out[bufsize];
	int inpos=0;
	int outpos=0;
	out[0]=0;

#undef transform
#define transform(a,b) if(!strcmp(&out[outpos],a)){\
					out[outpos-1]=b;\
					out[outpos]=0;\
				}
	while(in[inpos]){
		switch(in[inpos]){
			case '&': {int epos=outpos+1;
				out[outpos++]=in[inpos];
				out[outpos]=0; 
				while(in[++inpos]!=';' && in[inpos]!=0 && in[inpos]!='<' )
					out[epos++]=in[inpos];
				out[epos]=0; 
				} 
				transform("amp",'&')
				else transform("gt",'>')
				else transform("lt",'<')
				else transform("quot",'"')
				else transform("apos",'\'')
				break;
			case ' ':/* white space */
			case '\t': case 13: case 10:
			if(!prefs.keepwhitespace){
				if(out[0] && out[outpos-1]!=' '){
					out[outpos++]=' ';
					out[outpos]=0;
				}
			} else  {
				/*if(out[0])*/{
					out[outpos++]=' ';
					out[outpos]=0;
				}
			}
				break;
			default:
				out[outpos++]=in[inpos];
				out[outpos]=0;
		}
		inpos++;
	}

	if(generic && out[0]=='<'){	
		memmove(&out[1],&out[0],bufsize-1);
		out[0]=' ';
	}
	
return out;
}

/*	single pass xml parser (c) Øyvind Kolås 2001 */
/*  if generic==0,.. import hnb DTD else import general XML */
Node *xml__import (Node *node, char *filename,int generic){
	char data[bufsize+5];	 /* data to store in tree */
	int dpos=0;			/* position in data */
	char buf[bufsize]; /* input buffer */
	int cnt=0;			/* position in input buffer */	

	int in_comment=0;	/* variables keeping track of */
	int got_data=0;		/* where in the xml file we are*/
	int in_tag=0;
	int in_spec_tag=0;	/* we're in a tag starting with ! or ?*/

	int level=0;		/* keeps track of nesting, for the tree insertion */	
	int swallow=0;
	int flags=0;		/* for usage with hnb's dtd only */
	int priority=0;		/* --"-- */
	import_state_t	ist;
	
	int line=0;
	
	FILE *file;

	file = fopen (filename, "r");
	if (file == NULL)
		return (node);


	data[0]=0;

	/*npos = node;*/
	/*startlevel = nodes_left (node);*/
	init_import(&ist, node);
	
	while (fgets (buf, bufsize, file) != NULL) {	/* fill buffer */
		line++;
		cnt=0;
		if(prefs.debug)fprintf(stderr,"[%4i,%4i,%i]",line,dpos,level);
		while(buf[cnt]){		/* process buffer */
			if(dpos>bufsize){
				fprintf(stderr,"\n\n!!!! input buffer exceeded in line '%i', reducing 50 bytes, data will be lost !!! \n\n",line);
					dpos-=50;
					data[dpos]=0;
			}
			if(prefs.debug)fprintf(stderr,"%c",buf[cnt]);
			if (in_tag){					/* <in tag */
				switch(buf[cnt]){
									/*ignore white space*/
					case ' ':case '\t':case 13:case 10:
						if(data[1]&&data[dpos-1]!=' '){
							data[dpos++]=' ';
							data[dpos]=0;
						}
						break;
					case '>':		/*tag ends*/
						if(data[dpos-1]==' '){
							data[dpos-1]='>';
						} else {
							data[dpos++]=buf[cnt];
							data[dpos]=0;
						}
						if(generic)import_node(&ist,level,0,0,data);
						if(data[strlen(data)-2]!='/')
							level++;
						data[dpos=got_data=0]=0;
						in_tag=0;
						break;
					case '=':		
						if(!generic){	/*get attributes*/
							if(buf[cnt+2]=='y')flags=F_todo+F_done;
							else if(buf[cnt+2]=='n')flags=F_todo;
							else priority=(unsigned char)buf[cnt+2]-48;
						}
					case '/':			/* closing time? */
						if(dpos==1){ 
							level--;
							swallow='>';
							in_tag=0;
							data[dpos=got_data=0]=0;
							break;
						}
					case '!':
						if(dpos==1&&buf[cnt+1]=='-'&&buf[cnt+2]=='-'){
							data[dpos++]=buf[cnt++];
							data[dpos++]=buf[cnt++];
							data[dpos++]=buf[cnt];
							data[dpos]=0;
							in_tag=0;
							in_comment=1;
							if(prefs.debug)fprintf(stderr,"--");
						}
					case '?':	/* entity? tag? */
						if(dpos==1){
							in_spec_tag=1;
							in_tag=0;
							data[dpos++]=buf[cnt];
							data[dpos]=0;
							break;
						}
					default:
						data[dpos++]=buf[cnt];
						data[dpos]=0;
						break;
				}
			} else if (swallow){
				if(buf[cnt]==swallow){
					swallow=0;
				}
			}else if (in_spec_tag) {		/* <?|! in tag thingum>*/
				switch(buf[cnt]){
					case '<':in_spec_tag++;		/* count <>'s*/
						data[dpos++]=buf[cnt];
						data[dpos]=0;
						break;
					case '>':in_spec_tag--;
					default:
						data[dpos++]=buf[cnt];
						data[dpos]=0;
						break;
				}
				if(!in_spec_tag){
						if(generic)
							import_node(&ist,level,0,0,data);
						data[dpos=got_data=0]=0;
				}
			} else if(in_comment) {		/* <!-- in comment --> */
				data[dpos++]=buf[cnt];
				data[dpos]=0;
				got_data=1;
				if(buf[cnt]=='-' && buf[cnt+1]=='-' && buf[cnt+2]=='>'){
					in_comment=0;
					data[--dpos]=0;
					cnt+=2;
					if(prefs.debug)fprintf(stderr,"->");
					if(generic && got_data)
						import_node(&ist,level,0,0,data);
					
					data[dpos=got_data=0]=0;
				}
			} else { 						/* > outside <tags*/
				if(buf[cnt]=='<'){/*tag start*/
					if(got_data){
						import_node (&ist,level-(generic?0:3),
							flags,priority,
							xml_unquote(data,generic));
						flags=priority=0;
					}
					data[dpos=got_data=0]=0;
					in_tag=1;
					dpos=0;
					data[dpos++]=buf[cnt];
					data[dpos]=0;
				} else {
					data[dpos++]=buf[cnt];
					data[dpos]=0;
					if(buf[cnt]!=' '		/* pure whitespace isn't data */
						&&buf[cnt]!='\t'
						&&buf[cnt]!=10
						&&buf[cnt]!=13)	got_data=1;
				}
			}
			cnt++;
		}
	}
	fclose (file);
	if(level)fprintf(stderr,level>0?"%i tags not closed\n":"%s too many tags closed\n",level);
	/* remove temporary node, if tree was empty */
	if (node_getflag (node, F_temp))
		node = node_remove (node);

	return (node);
}

Node *hnb_import (Node *node, char *filename){
	return xml__import (node, filename,0);
}

Node *xml_import (Node *node, char *filename){
	Node *tnode;
	tnode=xml__import (node, filename,1);
	if(prefs.xml_cuddle)tnode=gxml_cuddle_nodes(tnode);
	return tnode;
}

#ifdef USE_LIBXML
void
libxml_export_data(FILE * file, char *data)
{
	int i = 0;
	while(data[i])
	{
		switch(data[i])
		{
			case '<':
				fprintf(file,"&lt;");
				break;
			case '>':
				fprintf(file,"&gt;");
				break;
			case '&':
				fprintf(file,"&amp;");
				break;
			case '\'':
				fprintf(file,"&apos;");
				break;
			case '"':
				fprintf(file,"&quot;");
				break;
			default:
				fputc(data[i], file);
				break;
		}
		i++;
	}
	fprintf(file,"\n");
}

static char *
libxml_export_node_pre (FILE * file, Node *node, int flags, char *data_orig)
{
	int i = 1;
	int j;
	char *data;
	char prefix[] = "(hnbnode) ";
	unsigned char priority;
	if(data_orig[0] != '(' && flags&F_todo)
	{
		data = (char *)malloc(strlen(data_orig) + strlen(prefix) + 1);
		sprintf(data,"%s%s",prefix,data_orig);
	}
	else
	{
		data = strdup(data_orig);
	}
	if(data[0] == '(')
	{
		while(data[i] != ')')
		{
			if(data[i] == 0)
			{
				libxml_export_data(file,data);
				return data;
			}
			i++;
		}
		data[i] = 0;
		j = i + 1;
		while(data[j] == ' ' || data[j] == '\t')
		{
			j++;
		}
		priority = node_getpriority(node);
		fprintf (file, "<%s title=\"%s\" ",&data[1], &data[j]);
		if(flags&F_todo)
		{
			fprintf (file, "todo=\"%s\" ", (flags&F_done)?"done":"");
		}
		if(priority != 0)
		{
			fprintf (file, "priority=\"%i\" ", priority);
		}
		fprintf (file, ">\n");
	}
	else
	{
		libxml_export_data(file,data);
	}
	return data;
}

static void
libxml_export_node_post (FILE * file, int flags, char *data)
{
	int i = 1;
    if(data[0] == '(')
    {
		while(data[i])
		{
			if(data[i]==' ' || data[i] == '\t') { data[i]=0;break; }
			i++;
		}
        fprintf (file, "</%s>\n", &data[1]);
    }
}

void libxml_rec(Node * node, FILE *file)
{
	int i;
	Node *tnode;
	int flags;
	char *data;
	char *moddata;
	flags = node_getflags (node);
	data = node_getdata (node);

	moddata = libxml_export_node_pre(file, node, flags, data);
	tnode = node_right(node);
	while(tnode)
	{
		libxml_rec(tnode, file);
		tnode = node_down(tnode);
	}
	libxml_export_node_post(file, flags, moddata);
	free(moddata);
}

void libxml_export(Node * node, char *filename)
{
	FILE *file;
    file = fopen(filename, "w");
    while(node)
    {
        libxml_rec(node, file);
        node = node_down(node);
    }
    fclose(file);
}

Node *libxml_populate(import_state_t *is, xmlNodePtr root, int level)
{
	xmlNodePtr cur = root->children;
	xmlAttrPtr prop;
	char *data, *s;
	int flags = 0;
	char attrstring[bufsize];
	char *sbuf = NULL;
	int len = 1;
	static char *notitle = "";
	static char *delim = "\n";
	unsigned char priority = 0;
	data = notitle;
	while(cur)
	{
		if(!xmlIsBlankNode(cur))
		{
			if(xmlNodeIsText(cur))
			{
				s = strdup((char *)cur->content);
				data = strtok(s, delim);
				while(data)
				{
					import_node (is, level, flags, 0, data);
					data = strtok(NULL,delim);
				}
				free(s);
			}
			else
			{


				
				priority = 0;
				sbuf = (char *)malloc(1);  sbuf[0]=0;
				prop = cur->properties;
				while(prop)
				{
					if(!strcmp("title", prop->name))
					{
						data = xmlGetProp(cur,XMLCHAR("title"));
					}
					else if(!strcmp("priority", prop->name))
					{
						priority = (unsigned char)atol(xmlGetProp(cur,XMLCHAR("priority")));
					}
					else if(!strcmp("todo", prop->name))
					{
						flags |= F_todo;
						if(!strcmp("done",xmlGetProp(cur, XMLCHAR("todo"))))
						{
							flags |= F_done;
						}
					}
					else
					{
						snprintf(attrstring, bufsize, " %s=\"%s\"",prop->name,
							xmlGetProp(cur, XMLCHAR(prop->name)));
						len += strlen(attrstring);
						sbuf = (char *)realloc(sbuf, len);
						strcat(sbuf, attrstring);
					}
					prop = prop->next;
				}


#if 0
				if(xmlGetProp(cur, XMLCHAR("title")))
				{
					data = xmlGetProp(cur,XMLCHAR("title"));
				}
				else
				{
					data = notitle;
				}
				if(xmlGetProp(cur, XMLCHAR("todo")))
				{
					flags |= F_todo;
					if(!strcmp("done",xmlGetProp(cur, XMLCHAR("todo"))))
					{
						flags |= F_done;
					}
				}
#endif

				s = (char *)malloc(strlen(cur->name) + strlen(data) + strlen(sbuf) + 4);
				if(strcmp("hnbnode",cur->name))
				{
					sprintf(s, "(%s%s) %s", cur->name, sbuf, data);
				}
				else
				{
					sprintf(s, "%s", data);
				}
				import_node (is,level, flags, priority, s);
				free(s);
				flags = 0;
			}
			libxml_populate(is,cur, level+1);
		}
		cur = cur->next;
	}

	return;
}

Node *libxml_import(Node *node,char *filename)
{
	FILE *file;
	char *data;
	unsigned int bsize;
	import_state_t	ist;

	bsize = 6;
	data = (char *)malloc(bsize+1);
	sprintf(data,"<hnb>\n");
	file = fopen(filename, "r");
	if (file == NULL) { return node; }


	while(!feof(file) && !ferror(file) )
	{
		data = (char *)realloc((void *)data, bsize + bufsize);
		bsize += fread (&data[bsize], 1, bufsize, file);
	}
	fclose(file);

	data = (char *)realloc((void *)data, bsize + 8);
	sprintf(&data[bsize],"</hnb>\n");
	bsize+=8;

	xmldoc = xmlParseMemory(data,bsize);

	init_import(&ist, node);

	libxml_populate(&ist, xmldoc->children, 0);
	if (node_getflags (node) & F_temp)
		node = node_remove (node);
	return (node);
}

#endif

void (* ptr_export)(Node *,char *) = NULL;
Node * (* ptr_import)(Node *,char *) = NULL;

