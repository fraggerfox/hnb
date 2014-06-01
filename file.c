#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "node.h"
#include "tree.h"

#define bufsize 4096

static Node *npos;				/* pointer within file to keep track of where we are, whilst importing */
static int startlevel;			/* says how deep in the tree we started importing */

void import_node (int level, int flags, char *data)
{
	int nl;

	level += startlevel;

	while ((nl = nodes_left (npos)) > level)
		npos = node_left (npos);

	if (nl == level)
		npos = node_insert_down (npos);

	if (nl < level)
		npos = node_insert_right (npos);

	node_setflags (npos, flags);
	node_setdata (npos, data);
}

void import_byte (byte)
{

#define xlevel1 0
#define xlevel2 1
#define xflags1 2
#define xflags2 3
#define xdata   4

	static int expect = xlevel1;
	static int level;
	static int flags;
	static char *data;
	static int pos;

	switch (expect) {
		case xlevel1:
			level = byte;
			expect = xlevel2;
			break;
		case xlevel2:
			level = level + byte * 256;
			expect = xflags1;
			break;
		case xflags1:
			flags = byte;
			expect = xflags2;
			break;
		case xflags2:
			flags = flags + byte * 256;
			expect = xdata;
			data = (char *) malloc (32);
			pos = 0;
			data[pos] = 0;
			break;
		case xdata:
			if (byte) {
				data[pos] = byte;
				data[pos + 1] = 0;
				pos++;
				if ((pos & 31) == 31) {	/* increase the buffer every 32 chars */
					data = (char *) realloc (data, pos + 32);
				};
			} else {
				import_node (level, flags, data);
				free (data);
				expect = xlevel1;
			}
			break;
	};
}

#ifndef WIN32
Node *import_db (Node *node, char *filename)
{
	char *buf;
	int file;
	int pos, end;
	int eof = 0;

	npos = node;
	startlevel = nodes_left (node);

	buf = (char *) malloc (bufsize);
	file = open (filename, O_RDONLY);

	while (!eof) {
		end = read (file, &buf[0], bufsize);
		if (end != bufsize)
			eof = 1;
		pos = 0;

		while (pos < end)
			import_byte (buf[pos++]);
	};
	free (buf);
	close (file);
	if (node_getflags (node) & F_temp)
		node = node_remove (node);
	return (node);
}

void export_node (int file, int level, int flags, char *data)
{
	char temp;

	temp = level & 255;
	write (file, &temp, 1);
	temp = (level / 256) & 255;
	write (file, &temp, 1);
	temp = flags & 255;
	write (file, &temp, 1);
	temp = (flags / 256) & 255;
	write (file, &temp, 1);
	write (file, data, strlen (data) + 1);
}

void export_db (Node *node, char *filename)
{
	Node *tnode;
	int level, flags, startlevel;
	int file;
	char *data;

	file = creat (filename, 0660);
	startlevel = nodes_left (node);

	tnode = node;

	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		data = node_getdata (tnode);
		export_node (file, level, flags, data);

		tnode = node_recurse (tnode);
	};

	close (file);
}

#endif

static void ascii_export_node (FILE * file, int level, int flags, char *data)
{
	int cnt;

	for (cnt = 0; cnt < level; cnt++)
		fprintf (file, "\t");

	if (flags & F_todo) {		/* print the flags of the current node */
		if (flags & F_done)
			fprintf (file, "[X]");
		else
			fprintf (file, "[ ]");
	} else {					/* check wether we must escape the first char of the string */
		if (data[0] == '[')
			fprintf (file, "[]");
	}

	fprintf (file, "%s\n", data);
}

Node *ascii_import (Node *node, char *filename)
{
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

		/*strip newline from string, and if dosmode file,.. also strip  the carrier return  */
		data[strlen (data) - 1] = 0;
		if (data[strlen (data) - 1] == 13)
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

		import_node (level, flags, &data[level + cnt]);
	}

	fclose (file);

	if (node_getflags (node) & F_temp)
		node = node_remove (node);
	return (node);
}

void ascii_export (Node *node, char *filename)
{
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


void html_export (Node *node, char *filename)
{
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

void latex_export (Node *node, char *filename)
{
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *data;
	FILE *file;

	file = fopen (filename, "w");
	startlevel = nodes_left (node);

	tnode = node;
	lastlevel = 0;
	fprintf (file, "\\documentclass[a4paper,11pt]{article}\n\
\\usepackage[T1]{fontenc}\n\
\\usepackage[latin1]{inputenc}\n\
\n\
%%latex file exported from hnb\n\
\n\
\\begin{document}\n\
\n\
\\begin{itemize}\n");

	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		data = node_getdata (tnode);

		if (level > lastlevel) {
			for (cnt = 0; cnt <= level - 1; cnt++)
				fprintf (file, "\t");
			fprintf (file, "  \\begin{itemize}\n");
		}
		if (level < lastlevel) {
			int level_diff = lastlevel - level;

			for (; level_diff; level_diff--) {
				for (cnt = 0; cnt <= level + level_diff - 1; cnt++)
					fprintf (file, "\t");
				fprintf (file, "  \\end{itemize}\n\n");
			}
		}
		for (cnt = 0; cnt <= level; cnt++)
			fprintf (file, "\t");

		fprintf (file, "\\item %s%s\n",
				 (flags & F_todo ? (flags & F_done ? "(X) " : "(\\ ) ") : ""),
				 data);

		lastlevel = level;
		tnode = node_recurse (tnode);
	}

	level = 0;

	{
		int level_diff = lastlevel - level;

		for (; level_diff; level_diff--) {
			for (cnt = 0; cnt <= level + level_diff - 1; cnt++)
				fprintf (file, "\t");
			fprintf (file, "  \\end{itemize}\n\n");
		}
	}

	fprintf (file, "\\end{itemize}\n\n\\end{document}");
	fclose (file);

	{
		char cmd_buf[200];

		sprintf (cmd_buf, "mv %s hnb.tmp.tex &&\
cat hnb.tmp.tex | sed -e s/_/\\\\\\\\_/g -e \"s/&/\\\\\\\\&/g\"> %s&&\
rm hnb.tmp.tex", filename, filename);
		system (cmd_buf);
	}
}

Node *help_import (Node *node)
{
	npos = node;
	startlevel = nodes_left (node);
#include "tutorial.inc"

	if (node_getflags (node) & F_temp)
		node = node_remove (node);
	return (node);
}

void help_export (Node *node, char *filename)
{
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *data;
	FILE *file;

	file = fopen (filename, "w");
	startlevel = nodes_left (node);

	tnode = node;
	lastlevel = 0;
	fprintf (file, "#define i(a,b,c) import_node(a,c,b)\n\n");
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		data = node_getdata (tnode);

		for (cnt = 0; cnt < level; cnt++)
			fprintf (file, "\t");

		fprintf (file, "i(%i,\"%s\",%i);\n", level, data, flags);

		lastlevel = level;
		tnode = node_recurse (tnode);
	};
	level = 0;

	fclose (file);
}
