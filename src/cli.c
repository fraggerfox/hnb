/*
 * cli.c -- a small interpreter for hnb
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

#include "tree.h"
#include "file.h"
#include "path.h"
#include "prefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* strips the ending node off a path */
char *path_strip(char *path){			/* add double // escaping when node2path gets it */
	
	int j=strlen(path);
	
	while(j>0){
		if(path[j-1]=='/'){
			path[j]=0;
			return path;
		}
		j--;
	};
	return path;
}

Node* help(Node *pos, char *params);

Node* dummy(Node *pos, char *params){
	return pos;
}

Node* eCho(Node *pos, char *params){
	printf("%s\n",params);
	return pos;
}

Node* save(Node *pos, char *params){
	if (prefs.db_file[0] != (char) 255) {
		switch(prefs.format){
			case FORMAT_ASCII:
				ascii_export ((Node *) node_root (pos), prefs.db_file);
				break;
			case FORMAT_XML:
				xml_export ((Node *) node_root (pos), prefs.db_file);								
				break;
			case FORMAT_GXML:
				gxml_export ((Node *) node_root (pos), prefs.db_file);								
				break;
			}				

		fprintf (stderr,"wrote stuff to '%s'\n", prefs.db_file);
	}
	
	return pos;
}

Node* export(Node *pos, char *params){
	
	int format=0; /* 0 = ascii */
	
	if(params[0]=='-'){ /* parse options */
		int j=0;
		while(params[j]!=0 && params[j]!=' '){
			j++;
			if(params[j]=='a' || params[j]=='A')
				format=FORMAT_ASCII;
			if(params[j]=='h' || params[j]=='H')
				format=FORMAT_HTML;
			if(params[j]=='x' || params[j]=='X')
				format=FORMAT_XML;
			if(params[j]=='g' || params[j]=='G')
				format=FORMAT_GXML;
		}
		params= &params[j];
		if(params[0]==' ')
			params++;
	}
	
	if(!params[0]){
		fprintf(stderr,"just must specify a file to write the exported data to.\n");
		return pos;
	}
	
	switch(format){
		case FORMAT_ASCII:
			ascii_export (node_top (pos), params);
			break;
		case FORMAT_HTML:
			html_export (node_top (pos), params);		
			break;
		case FORMAT_XML:
			xml_export(node_top(pos),params);
			break;
		case FORMAT_GXML:
			gxml_export(node_top(pos),params);
			break;			
	}
	
	return pos;
}

Node* import(Node *pos, char *params){
	
	int format=0; /* 0 = ascii */
	
	if(params[0]=='-'){ /* parse options */
		int j=0;
		while(params[j]!=0 && params[j]!=' '){
			j++;
			if(params[j]=='a' || params[j]=='A')
				format=FORMAT_ASCII;
			if(params[j]=='x' || params[j]=='X')
				format=FORMAT_XML;
			if(params[j]=='g' || params[j]=='G')
				format=FORMAT_GXML;
		}
		params= &params[j];
		if(params[0]==' ')
			params++;
	}
	
	if(!params[0]){
		fprintf(stderr,"just must specify a file to import\n");
		return pos;
	}
	
	switch(format){
		case FORMAT_ASCII:
			pos=ascii_import (node_bottom (pos), params);
			break;
		case FORMAT_XML:
			pos=xml_import(node_bottom(pos),params);
			break;
		case FORMAT_GXML:
			pos=gxml_import(node_bottom(pos),params);
			break;
	}
	
	return pos;
}

Node* add(Node *pos, char *params){
	Node *tnode;
	if(!params[0]){
		fprintf(stderr,"empty node added\n");
	}
	
	tnode=node_insert_down(node_bottom(pos));
	node_setdata(tnode,params);
	return pos;
}

Node* addc(Node *pos, char *params){
	Node *tnode;
	
	int j=0;
	char par[100];
	char *child;
	strncpy(par,params,100);
	par[99]=' ';
	while(par[j]!=' ' && par[j]!=0)
		j++;
	par[j]=0;
	
	child=&params[j+1];
	if(strlen(par)==strlen(params))
		child="";
	
	tnode=node_exact_match(par,pos);
	if(!tnode){
		fprintf(stderr,"specified parent not found");
		return(pos);
	}
	if(node_right(tnode)){
		fprintf(stderr,"failed, node already had a child");
		return(pos);
	}
	
	if(!child[0]){
		fprintf(stderr,"empty node added\n");
	}
	
	tnode=node_insert_right(tnode);
	node_setdata(tnode,child);
	return pos;
}

Node *pwd(Node *pos,char *params){
	
	printf("%s\n",path_strip(node2path(pos)));
	return pos;
}

Node *cd(Node *pos,char *params){
	Node *tnode=pos;
	
	if(!strcmp(params,".."))
		params[0]=0;
	
	if(params[0]){
		tnode=path2node(params,pos);
		if(tnode){
			tnode=node_right(tnode);
		}
		if(!tnode){
			fprintf(stderr,"no such node\n");
			return pos;
		}
		return tnode;
	} else { /* go to parent */
		if(node_left(tnode)!=0)
			return(node_left(tnode));
	}
	
	return pos;
}

Node* sort(Node *pos, char *params){
	return node_sort_siblings (pos);
}

Node* rm(Node *pos, char *params){
	Node *tnode;
	int force=0;
	
	if(params[0]=='-'){ /* parse options */
		int j=0;
		while(params[j]!=0 && params[j]!=' '){
			j++;
			if(params[j]=='H' || params[j]=='h'){
				fprintf(stderr,"type '? rm' for help on rm\n");
				return(pos);
			}
			if(params[j]=='F' || params[j]=='f' || params[j]=='r')
				force=1;
		}
		params= &params[j];
		if(params[0]==' ')
			params++;
	}
	
	if(!params[0]){
		fprintf(stderr,"no node specified for removal\n");
		return(pos);
	}
	
	tnode=path2node(params,pos);
	if(!tnode){
		fprintf(stderr,"no such node\n");
		return pos;
	}

	if(node_right(tnode)){
		if(force){
			return(node_remove(tnode));
		} else {
			fprintf(stderr,"node has children force removal with 'rm -f'\n");
		}
	} else {
		return(node_remove(tnode));
	}
	
	return(pos);
}

Node* ls(Node *pos, char *params){
	Node *tnode;
	int recurse=0;
	int indicate_sub=0;
	int indicate_todo=0;
	int startlevel;
	
	if(params[0]=='-'){ /* parse options */
		int j=0;
		while(params[j]!=0 && params[j]!=' '){
			j++;
			if(params[j]=='H' || params[j]=='h'){
				fprintf(stderr,"type '? ls' for help on ls\n");
				return(pos);
			}
			if(params[j]=='R' || params[j]=='r')
				recurse=1;
			if(params[j]=='T' || params[j]=='t')
				indicate_todo=1;
			if(params[j]=='s' || params[j]=='S')
				indicate_sub=1;
		}
		params= &params[j];
		if(params[0]==' ')
			params++;
	}
	
	tnode=node_top(pos);
	if(params[0]){
		tnode=path2node(params,pos);
		if(tnode){
			tnode=node_right(tnode);
		} else {
			fprintf(stderr,"no such node\n");
			return pos;
		}
	}
	
	startlevel=nodes_left(tnode);
	while(tnode){
		int paren=0;
		
		if(recurse){
			int j;
			for(j=nodes_left(tnode);j>startlevel;j--){
				printf("\t");
			}
		}
		
		fprintf(stdout,"%s",node_getdata(tnode));
	
	if(indicate_sub)
		if(node_right(tnode)){
			printf("\t(.. ");
				paren=1;
			}
	
	if(indicate_todo)
		if(node_getflag(tnode,F_todo)){
			if(!paren)printf("\t(");
				if(node_getflag(tnode,F_done)){		
					printf("done");
				} else {
					printf("not done");
				}
				paren=1;
			}
	
	if(paren){
		fprintf(stdout,")\n");
		} else {
			fprintf(stdout,"\n");
		}
		
		if(recurse){
			tnode=node_recurse(tnode);
			if(nodes_left(tnode)<startlevel)
				tnode=0;
		} else {
			tnode=node_down(tnode);		
		}
	};
	return pos;
}

typedef struct{
	char    *name;						/* name of the variable */
	int*    integer;					/* pointer to integer (set to NULL if string)*/
	char*   string;						/* pointer to string (set to NULL if integer) */
	char      *help;					/* helptext for this variable */
} VariableT;

VariableT Variable[]={
	{"eleet",			&prefs.eleet_mode,		NULL,	"display chars in eLi7e mode"},
	{"debug",			&prefs.view_debug,		NULL,	"view debug information"},
	{"format",			&prefs.format,			NULL,	"the format of this file"},
	{"def_format",		&prefs.def_format,		NULL,	"default format (and format of default db)"},	
	{"collapse_mode",	&prefs.collapse_mode,	NULL,	"controls how the information is collapsed"},
	{"help_level",		&prefs.help_level,		NULL,	"amount of help to give"},
	{NULL,NULL,NULL,NULL}/*termination*/
};

Node *set(Node *pos,char *params){
	int j;
	if(!params[0]){
		j=0;
		while(Variable[j].name!=NULL){
			if(Variable[j].integer!=NULL)
				fprintf(stderr,"%s\t[%i]\t- %s\n",Variable[j].name,*Variable[j].integer,Variable[j].help);
			else if(Variable[j].string!=NULL)
				fprintf(stderr,"%s\t[%s]\t- %s\n",Variable[j].name,Variable[j].string,Variable[j].help);
			j++;
		};
		return pos;
	} else 	{
		int j=0;
		char var[20];
		char *value;
		strncpy(var,params,20);
		var[19]=' ';
		while(var[j]!=' ' && var[j]!=0)
			j++;
		var[j]=0;
	
		value=&params[j+1];
		if(strlen(var)==strlen(params))value="";
		
		j=0;
		while(Variable[j].name!=NULL){
			if(!strcmp(var,Variable[j].name)){
				if(Variable[j].integer!=NULL){
					if(value[0]){
						*Variable[j].integer=atoi(value);
						fprintf(stderr,"%s\t set to [%i]\t\n",Variable[j].name,*Variable[j].integer);
					} else {
						fprintf(stderr,"%s\t[%i]\t- %s\n",Variable[j].name,*Variable[j].integer,Variable[j].help);
					}
				} else if(Variable[j].string!=NULL){
					if(value[0]){
						strcpy(Variable[j].string,value);
						fprintf(stderr,"%s\t set to [%s]\t\n",Variable[j].name,Variable[j].string);
					} else {
						fprintf(stderr,"%s\t[%s]\t- %s\n",Variable[j].name,Variable[j].string,Variable[j].help);
					}
				}
				return pos;
			}
			j++;
		}
		fprintf(stderr,"no such variable '%s'\n",var);
		return pos;
	}
}

typedef struct{
	char    *name;								/* what the user types */
	Node*    (*func) (Node *pos,char *params);	/* function that is the command */
	char	 *synopsis;							/* usage for the command */
	char     *help;								/* helptext for this command */
} CommandT;

CommandT Command[]={
	{"?"	,help	,"? [command]",
		"see the entry for 'help'"},
	{"add"	,add	,"add <string>",
		"adds a node containing string at bottom"},
	{"addc"	,addc	,"addc <parent> <string>",
		"creates a child for specified parent node"},
	{"cd"	,cd		,"cd <path>",
		"changes location in tree"},	
	{"echo"	,eCho	,"echo <string>",
		"echo's the string to screen."},	
	{"export",export,"export [-ahxg] <file>\n\noptions: a = ascii(default), h = html, x = xml, g = general xml",
		"exports the database from the current level and down to a file."},	
	{"help"	,help	,"help [command]",
		"displays available commands and help for them"},
	{"import",import,"import [-axg] <file>\n\noptions: a = ascii(default), x = xml, g = general xml",
		"imports the specified file, at the current level."},	
	{"ls"	,ls		,"ls [-Rst] [path]\n\noptions: R = recurse, s = indicate subnodes, t = indicate todo status",
		"lists the nodes on this level, or under the node specified\n\t  as a parameter"},
	{"rm"	,rm		,"rm [-f] path\n\noptions: f = force removal of node with children"	,"removes a node"},
	{"pwd"	,pwd	,"pwd",
		"returns the current path"},
	{"q"	,dummy	,"q",
		"see the entry for 'quit'"},
	{"quit"	,dummy	,"quit",
		"exits hnb, (no confirmation, no saving)"},
	{"set"	,set	,"set [<variable> [value]]",
		"sets a variable, without argument shows all variables"},
	{"save"	,save	,"save",
		"saves the database"},
	{"sort"	,sort	,"sort",
		"sorts the nodes in this level"},
	{NULL,NULL,NULL}
};

Node *help(Node *pos, char *params){
	if(params[0]==0){ /* show all help */
	  int j=0;
	
		printf("HELP:\n");	  
	  
		while(Command[j].name!=NULL){
			fprintf(stderr,"%s\t- %s\n",Command[j].name,Command[j].help);
			j++;
		};
	
	
	} else { /* show help for specified command */
		int j=0;
		printf("HELP for '%s'\n\n",params);
		
		while(Command[j].name!=NULL){
			if(!strcmp(params,Command[j].name)){
				fprintf(stderr,"%s\n\n%s\n\n",Command[j].synopsis,Command[j].help);
				return pos;
			}
			j++;
		};
		fprintf(stderr,"unknown command '%s'\n",params);
	}
	
	return pos;
}

Node *docmd(Node *pos,char * commandline){
	int j=0;
	char command[10];
	char *params;
	strncpy(command,commandline,10);
	command[9]=' ';
	while(command[j]!=' ' && command[j]!=0)
		j++;
	command[j]=0;
	
	params=&commandline[j+1];
	if(strlen(commandline)==strlen(command))params="";
	
	j=0;
	while(Command[j].name!=NULL){
		if(!strcmp(command,Command[j].name)){
			pos=Command[j].func(pos,params);
			return pos;
		}
		j++;
	};
	
	printf("unknown command '%s' type '?' to see allowed commands.\n",command);
	return pos;
}

Node *cli(Node *pos){
	char commandline[4096];
	fprintf(stderr,"Welcome to %s %s\ntype ? or help for more information\n",PACKAGE, VERSION);
	
	do{
		fprintf(stderr,"%s>",path_strip(node2path(pos)));
		fgets(commandline,4096,stdin);
		commandline[strlen(commandline)-1]=0;
		pos=docmd(pos,commandline);
	} while (strcmp(commandline,"quit") && strcmp(commandline,"q"));
	return pos;
}
