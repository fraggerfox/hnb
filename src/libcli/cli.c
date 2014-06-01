/* libcli -- a small commandline interpreter libraray
 * Copyright (C) 2002 Øyvind Kolås
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cli.h"

#define HIDE_NULL_HELP

/* TODO:

	allow removal of commands/variables
	scripts? (with simple flow-control?)


RESERVED IDENTIFIERS:
	for, if, else
	while, do
	exit
	
	{
		a=5
		for(b=0;b<5;b++)
			{
				if( a+b < 8)
					{
						echo("hey there");
					}
				if (! a )
					echo "a"
				else
					echo "b"
			}
	}	
*/
static char outbuf[256];

#ifdef WIN32
#define snprintf(a,b,args...) sprintf(a,args)
#endif

#define outf(args...)  \
     do{	\
	 	snprintf (outbuf, 256, args);\
		outbuf[255]=0;\
		cli_outfun(outbuf);\
	 }while(0)


static void default_output (char *data)
{
	printf ("%s", data);
}

static void default_unknown_command (char *commandline, void *data){
	char command[100];
	char *params;	
	cli_split(commandline, command, &params);
	outf ("unknown command '%s' type '?' to see allowed commands.\n",
		  command);
}

void (*cli_outfun) (char *) = default_output;
void (*cli_precmd) (char *) = NULL;
void (*cli_postcmd) (char *) = NULL;
void (*cli_unknown) (char *,void *) = default_unknown_command;
int cli_width = 40;

static inline int item_matches (const char *itemname);

typedef struct ItemT {
	char *name;					/* what the user types */
	int (*func) (char *params, void *data);	/* function that is the command */
	int *integer;				/* pointer to integer (set to NULL if string) */
	char *string;				/* pointer to string (set to NULL if integer) */
	char *help;					/* helptext for this command */
	int flags;
	struct ItemT *next;
} ItemT;

#define is_command(a) (a->func && (a->integer==NULL) && (a->string==NULL))
#define is_variable(a) (!is_command(a))

static ItemT *items = NULL;

void
cli_add_item (char *name,
		  int *integer, char *string,
		  int (*func) (char *params, void *data), char *help)
{
	ItemT *titem = items;

	if (item_matches (name) == 1) {
		outf ("WARNING: attempted to add item '%s' more than once\n", name);
		return;
	}

	if (!titem) {
		titem = items = malloc (sizeof (ItemT));
		titem->next = NULL;
	} else {
		ItemT *tmp;

		while (titem->next && ((strcmp ((titem->next)->name, name)) < 0)) {
			titem = titem->next;
		}

		tmp = titem->next;
		titem->next = malloc (sizeof (ItemT));
		titem = titem->next;
		titem->next = tmp;
	}

	titem->name = strdup (name);
	titem->func = func;
	titem->integer = integer;
	titem->string = string;
	if(help)titem->help = strdup (help);

	if (strcmp (items->name, titem->name) > 0) {
		ItemT *tmp = items;
		ItemT *tmp_next = titem->next;

		items = titem;
		items->next = tmp;
		items->next->next = tmp_next;
	}
}

static int help (char *params, void *data);
static int vars (char *params, void *data);
static int nop (char *params, void *data)
{
	return (int)data;
}

static int inited = 0;
static void init_cli (void)
{
	cli_add_command ("quit", nop, "quits the application");
	cli_add_command ("?", help, "? - this listing");
	cli_add_command ("show_vars", vars, "show all variables");
	inited = 1;
}

int cli_calllevel=0;

int cli_docmd (char *commandline, void *data)
{
	char command[40];
	char *params;
	ItemT *titem = items;
	int ret=(int)data;
	cli_calllevel++;

	if (cli_precmd)
		cli_precmd (commandline);

	if (!inited) {
		init_cli ();
		titem = items;
		inited = 1;
	}

	if(commandline[0]=='\0')
		return ret;

	while(*commandline==' ' || *commandline=='\t')commandline++;
	
	cli_split(commandline, command, &params);

	while (titem) {
		if (!strcmp (command, titem->name)) {
			if (is_command (titem)) {

				ret=titem->func (params, data);

				if (cli_postcmd)
					cli_postcmd (commandline);
				cli_calllevel--;
				return ret;
			} else if (is_variable (titem)) {
				if (!params[0]) {
					if (titem->string) {
						outf ("%s\t[%s]\t- %s\n", titem->name,
							  titem->string, titem->help);
					} else if (titem->integer) {
						outf ("%s\t[%i]\t- %s\n", titem->name,
							  *titem->integer, titem->help);
					} else {
						outf ("%s\tis a broken variable\n", titem->name);
					}
				} else {
					if (titem->integer)
						*titem->integer = atoi (params);
					if (titem->string)
						strcpy (titem->string, params);
					if (titem->func)
						ret=titem->func (params, data);
				}
				if (cli_postcmd)
					cli_postcmd (commandline);
				cli_calllevel--;
				return ret;
			}
		}
		titem = titem->next;
	}
	if(cli_unknown)
		cli_unknown(commandline,data);	
	if (cli_postcmd)
		cli_postcmd (commandline);
	cli_calllevel--;
	return ret;
}

static char newcommand[100];

static inline int item_matches (const char *itemname)
{
	int matches = 0;
	ItemT *titem = items;

	while (titem) {
		if (!strncmp (itemname, titem->name, strlen (itemname)))
			matches++;
		titem = titem->next;
	}
	return matches;
}

/* outf with wordwrap */
static void bufcat (const char *string)
{
	static char catbuf[100] = "";

	if (string && (strlen (string) + strlen (catbuf) < cli_width)
		&& (strlen (string) + strlen (catbuf) < 99)) {
		strcat (catbuf, string);
	} else {
		if (catbuf[0]) {
			outf ("%s", catbuf);
			catbuf[0] = 0;
		}
	}
}

char *cli_complete (const char *commandline)
{
	int matches = 0;

	strncpy (newcommand, commandline, 99);
	newcommand[99] = 0;

	if (commandline[0]) {
		matches = item_matches (newcommand);

		if (matches == 1) {
			ItemT *titem = items;

			while (titem) {
				if (!strncmp (newcommand, titem->name, strlen (newcommand))) {
					int pos;

					strcpy (newcommand, titem->name);
					pos = strlen (newcommand);
					newcommand[pos++] = ' ';
					newcommand[pos] = '\0';
					break;
				}
				titem = titem->next;
			}
		} else if (matches > 1) {
			ItemT *titem = items;

			bufcat ("matches: ");
			while (titem) {
				if (!strncmp (newcommand, titem->name, strlen (newcommand))) {
					bufcat (titem->name);
					bufcat (" ");
				}
				titem = titem->next;
			}
			bufcat (NULL);
			while (item_matches (newcommand) == matches) {
				ItemT *titem = items;

				while (titem) {
					int len = strlen (newcommand);

					if (!strncmp (newcommand, titem->name, len)) {

						strcpy (newcommand, titem->name);
						newcommand[len + 1] = '\0';
						if(!strcmp(newcommand,titem->name)){
							return newcommand;
						}
						break;
					}
					titem = titem->next;
				}
			}
			newcommand[strlen (newcommand) - 1] = '\0';
		} else {
			outf ("no match");
		}
	}

	return newcommand;
}

/** internal commands */



static int help (char *params, void *data)
{
	if (params[0] == 0) {		/* show all help */
		ItemT *titem = items;

		outf ("available commands:\n");

		while (titem) {
		#ifdef HIDE_NULL_HELP
			if(titem->help)
		#endif
			if (is_command (titem))
				outf ("%14s %s\n", titem->name, titem->help);
			titem = titem->next;
		};
	} else {					/* show help for specified command */
		ItemT *titem = items;

		outf ("HELP for '%s'\n\n", params);

		while (titem) {
			if (is_command (titem)) {
				if (!strcmp (params, titem->name)) {
					outf ("%s\t- %s\n", titem->name, titem->help);
					return(int)data;
				}
			}
			titem = titem->next;
		}
		outf ("unknown command '%s'\n", params);
	}
	return(int)data;	
}

static int vars (char *params, void *data)
{
	ItemT *titem = items;

	outf ("all variables:\n");

	while (titem) {
		#ifdef HIDE_NULL_HELP
			if(titem->help)
		#endif
		if (is_variable (titem)) {
			if (titem->string) {
				outf ("%15s [%s]\t- %s\n", titem->name,
					  titem->string, titem->help);
			} else if (titem->integer) {
				outf ("%15s [%i]\t- %s\n", titem->name,
					  *titem->integer, titem->help);
			} else {
				outf ("%s\tis a broken variable\n", titem->name);
			}
		}
		titem = titem->next;
	}

	outf ("----------------\n");
	outf ("to change a variable: \"variablename newvalue\"\n");
	return(int)data;
}


void cli_split(char *orig, char *head, char **tail)				
	{												
		int j=0;									
		if(orig){									
			strncpy(head,orig,40);					
			head[39]=0;								
			while( head[j] != ' ' && head[j] != '\t' && head[j] != 0)	
				j++;								
			head[j]=0;								
			*tail=&orig[j+1];
			if(strlen( head) == strlen (orig) )		
				*tail="";							
		} else {
			head[0]=0;
			*tail="";
		}
	}

#include <stdio.h>

int cli_load_file(char *filename){
	char buf[255];
	FILE *file;

	file=fopen(filename,"r");
	if(!file){
		return -1;
	}

	while(fgets(buf,255,file)){
		char *c=strchr(buf,'\n');
		char *t;
		t=buf;
		if(c)*c='\0';
		if(*buf){
			while(*t==' ' || *t=='\t')t++;
/*			if(!strncmp(t,"macro_add ",10)){
				char *macro_name
			} else*/
			cli_docmd(buf,NULL);
		}
	}
	fclose(file);
	return 0;
}




