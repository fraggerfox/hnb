/*
 * xml_debug.c -- temporary small utility for debugging of the xml tokenizer
 *
 * Copyright (C) 2002 Øyvind Kolås <pippin@users.sourceforge.net>
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
 
 #include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "xml_tok.h"

char *t_name[] = {
	"none",
	"whitespace",
	"prolog    ",
	"dtd       ",
	"comment   ",
	"word      ",
	"tag       ",
	"closetag  ",
	"closeEtag ",
	"endtag    ",
	"att       ",
	"val       ",
	"eof       ",
	"entity    ",
	"error     "
};

void xmlparse (FILE * file_in, FILE * file_out, int cmpr)
{
	char *data;
	xml_tok_state *t;
	int type;

	t = xml_tok_init (file_in);
	while ((type = html_tok_get (t, &data)) != t_eof) {

		fprintf (file_out, "%s) [%s]\n", t_name[type], data);


		if (type == t_error)
			break;
	}
	xml_tok_cleanup (t);
}

int main (int argc, char **argv)
{
	xmlparse (stdin, stdout, argc - 1);
	return 0;
}
