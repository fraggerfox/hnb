/*
 * cli_tokenize.c -- tokenizer for libcli
 *
 * Copyright (C) 2003 Øyvind Kolås <pippin@users.sourceforge.net>
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


enum tokenize_string_states{
	s_start,
	s_wnew,
	s_wpp,
	s_wescape,
	s_waddchar,
	s_qnew,
	s_qpp,
	s_qescape,
	s_qaddchar,
	s_anew,
	s_app,
	s_aescape,
	s_aaddchar,
	s_swallowterm,
	s_returntoken,
	s_end
};


static char *first_nonspace(char *string){
	char *s=string;
	if(!s)return NULL;

	while(*s && isspace((unsigned)*s))s++;
	return s;
}


/*	tokenize string like the parameters of a unix shell tokenizes parameters for commands
	returns number of tokens in string
	if char **storage is non NULL it stores the data in the tokens there.
*/
static int internal_tokenize_string(char *string, char **storage){
	char *s=first_nonspace(string);
	int token_no=0;
	char token[4096];
	int token_pos=0;
	int state=s_start;

	while(2+2==4){ 
		switch(state){
			case s_start:
				switch(*s){
					case '\0':
						state=s_end;
						break;				
					case ' ':
						s++;
						state=s_start;
						break;
					case '\t':
						s++;
						state=s_start;
						break;
					case '\'':
						state=s_anew;
						break;
					case '\"':
						state=s_qnew;
						break;
					default:
						state=s_wnew;
						break;
				}
				break;
			case s_wnew:
				token_no++;
				if(*s=='\\'){
					token[token_pos=0]='\0';
					s++;
					state=s_wescape;
				} else {
					token[token_pos=0]='\0';
					state=s_wpp;
				}
				break;
			case s_wpp:
				switch(*s){
					case ' ':
					case '\t':
						s++;
					case '\0':						
						state=s_returntoken;
						break;
					case '\\':
						s++;					
						state=s_wescape;
						break;
					default:
						token[token_pos++]=*s;
						token[token_pos]='\0';
						s++;
						state=s_wpp;
						break;
				}
				break;
			case s_wescape:
				if(*s=='\0')
					state=s_returntoken;
				else
					state=s_waddchar;
				break;
			case s_waddchar:
				token[token_pos++]=*s;
				token[token_pos]  ='\0';
				s++;
				state=s_wpp;
				break;
			case s_anew:
				token_no++;
				token[token_pos=0]='\0';
				s++;
				state=s_app;
				break;
			case s_app:
				switch(*s){
					case '\'':
						s++;
						state=s_swallowterm;
						break;
					case '\0':
						s++;
						state=s_returntoken;
						break;
					case '\\':
						s++;
						state=s_aescape;
						break;
					default:
						token[token_pos++]=*s;
						token[token_pos]='\0';
						s++;
						state=s_app;
						break;
				}
				break;
			case s_aescape:
				if(*s=='\0')
					state=s_returntoken;
				else
					state=s_aaddchar;
				break;
			case s_aaddchar:
				token[token_pos++]=*s;
				token[token_pos]='\0';
				s++;
				state=s_app;
				break;
			case s_qnew:
				token_no++;
				token[token_pos=0]='\0';
				s++;
				state=s_qpp;
				break;
			case s_qpp:
				switch(*s){
					case '\"':
						s++;
						state=s_swallowterm;
						break;
					case '\0':
						s++;
						state=s_returntoken;
						break;
					case '\\':
						s++;
						state=s_qescape;
						break;
					default:
						token[token_pos++]=*s;
						token[token_pos]='\0';
						s++;
						state=s_qpp;
						break;
				}
				break;
			case s_qescape:
				if(*s=='\0')
					state=s_returntoken;
				else
					state=s_qaddchar;
				break;
			case s_qaddchar:
				token[token_pos++]=*s;
				token[token_pos]='\0';
				s++;
				state=s_qpp;
				break;
			case s_swallowterm:
				if(*s)
					s++;
				state=s_returntoken;
				break;
			case s_returntoken:
			
				if(storage)
					storage[token_no-1]=strdup(token);
				
				state=s_start;
				break;
			case s_end:
				return token_no;
				break;
			default:
				break;
		}
	}
	
	return -1; /* hell_froze over */
}


char **cli_tokenize(char *input, int *argc){
	int largc;
	char **largv;
	if(!input)
		return NULL;

	largc=internal_tokenize_string(input,NULL);
	largv=calloc( (largc+1) , sizeof(char *)); /* also allocate space for an extra NULL */
	internal_tokenize_string(input,largv);

	if(argc)*argc=largc;
	return largv;
}

void cli_free_tokenlist(char **argv){
	char **targv=argv;
	while(*targv){
		free(*targv);
		targv++;
	}
	free(argv);
}
#if 0

int main(int argc, char **argv){
	char *input="this is a test\\ test";
	int largc;
	char **largv;
	char **tlargv;

	printf("[%s]\n",input);
	
	largv=string_tokenize(input,&largc);
	
	printf("tokens:%i\n",largc);
	tlargv=largv;
	while(*tlargv)
		printf("[%s]\n",*(tlargv++));
	
	string_free_tokenlist(largv);
	
	printf("-----------------\n");
	return 0;
}

#endif

