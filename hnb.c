#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "node.h"
#include "tree.h"
#include "ui.h"
#include "file.h"
#include "version.h"
#include "path.h"

static void usage (const char *av0)
{
	char db_file[100];

#ifndef WIN32
	sprintf (db_file, "%s/.hnb", getenv ("HOME"));
#endif
#ifdef WIN32
	sprintf (db_file, "C:\\hnb_data");
#endif
	fprintf (stderr, "\nusage: %s [options] [file] \n", av0);
	fprintf (stderr, "\n\
Hierarchical NoteBook by Øyvind Kolås <pippin@users.sourceforge.net>\n\
It is distributed under the GNU General Public License\n\
\n\
if no file is specified, the file '%s' is loaded/created\n\
\n\
Options include:\n\
\n\
\t-h  or --help\t\tthis message\n\
\t-v  or --version\tprints the version\n\
\t-t  o --tutorial\tloads the tutorial instead of a database\n\
\n\n", db_file);
}

#define BUFFERLENGTH 4096
#warning BUFFERLENGTH is currently set to 4096,.. should be removed and replaced by rigid code

char input[BUFFERLENGTH];
Node *pos;


#define undefined_key(a,c)\
		    {if(c!=UI_IGNORE){char msg[80];\
	  	    sprintf(msg," No action assigned to '%s'(%id) in %s-mode",keyname(c),c,a);\
		    ui_draw(pos, msg, UI_MODE_ERROR);\
			}}\

#define info(a)\
		    {ui_draw(pos, a, UI_MODE_INFO);\
			}\

#define infof(a,b)\
		    {char msg[80];\
	  	    sprintf(msg,a,b);\
			ui_draw(pos, msg, UI_MODE_INFO);\
			}\


#ifdef WIN32

#undef undefined_key(a,c)
#define undefined_key(a,c)


#endif



void app_edit ()
{
	int c;
	int stop = 0;
	static int cursor_pos;
	static char *data_backup;
	static char input[BUFFERLENGTH];

	data_backup = pos->data;
	input[0] = 0;
	strcpy (&input[0], data_backup);
	pos->data = &input[0];
	cursor_pos = strlen (input);
	input[cursor_pos] = '_';
	input[cursor_pos + 1] = 0;
	input[cursor_pos + 2] = 0;
	c = 0;

	while (!stop) {
		ui_draw (pos, (char *) cursor_pos, UI_MODE_EDIT);
		c = ui_input ();
		switch (c) {
			case UI_RIGHT:
				if (cursor_pos < (strlen (input) - 1)) {
					char tmp;

					tmp = input[cursor_pos + 1];
					input[cursor_pos + 1] = '_';
					input[cursor_pos++] = tmp;
				};
				break;
			case UI_LEFT:
				if (cursor_pos) {
					char tmp;

					tmp = input[cursor_pos - 1];
					input[cursor_pos - 1] = '_';
					input[cursor_pos--] = tmp;
				};
				break;
			case UI_TOP:
			case UI_PUP:
				while (cursor_pos) {
					char tmp;

					tmp = input[cursor_pos - 1];
					input[cursor_pos - 1] = '_';
					input[cursor_pos--] = tmp;
				};
				break;
			case UI_BOTTOM:
			case UI_PDN:
				while (cursor_pos < (strlen (input) - 1)) {
					char tmp;

					tmp = input[cursor_pos + 1];
					input[cursor_pos + 1] = '_';
					input[cursor_pos++] = tmp;
				};
				break;
			case UI_ESCAPE:
				strcpy (&input[0], data_backup);
				pos->data = &input[0];
				cursor_pos = strlen (input);
				input[cursor_pos] = '_';
				input[cursor_pos + 1] = 0;
				stop = 1;
				break;
			case UI_UP:
			case UI_DOWN:
			case UI_ENTER:
				stop = 1;
				break;
			case UI_BACKSPACE:
			case UI_BACKSPACE2:
			case UI_BACKSPACE3:
				if (cursor_pos) {
					memmove (&input[cursor_pos], &input[cursor_pos + 1],
							 strlen (input) - cursor_pos);
					input[cursor_pos - 1] = '_';
					cursor_pos--;
				};
				break;
				/*ignored keypresses.. */
			case UI_INSERT:
				break;
			default:
				if (c > 31 && c < 255) {	/*  input for buffer */
					memmove (&input[cursor_pos + 1], &input[cursor_pos],
							 strlen (input) - cursor_pos + 1);
					input[cursor_pos] = c;

					cursor_pos++;
				} else {		/* undefined keycode */
					undefined_key ("edit", c);
				}
				break;
		};
	};

	/* position our cursor at the end */
	while (cursor_pos < (strlen (input) - 1)) {
		char tmp;

		tmp = input[cursor_pos + 1];
		input[cursor_pos + 1] = '_';
		input[cursor_pos++] = tmp;
	};
	input[cursor_pos] = 0;

	pos->data = data_backup;
	node_setdata (pos, input);
	stop = 0;
}


void old_app_edit ()
{
	int c;
	int stop = 0;
	static int cursor_pos;
	static char *data_backup;
	static char input[BUFFERLENGTH];

	data_backup = pos->data;
	input[0] = 0;
	strcpy (&input[0], data_backup);
	pos->data = &input[0];
	cursor_pos = strlen (input);
	input[cursor_pos] = '_';
	input[cursor_pos + 1] = 0;
	input[cursor_pos + 2] = 0;
	c = 0;

	while (!stop) {
		ui_draw (pos, (char *) cursor_pos, UI_MODE_EDIT);
		c = ui_input ();
		switch (c) {
			case UI_RIGHT:
				if (cursor_pos < (strlen (input) - 1)) {
					char tmp;

					tmp = input[cursor_pos + 1];
					input[cursor_pos + 1] = '_';
					input[cursor_pos++] = tmp;
				};
				break;
			case UI_LEFT:
				if (cursor_pos) {
					char tmp;

					tmp = input[cursor_pos - 1];
					input[cursor_pos - 1] = '_';
					input[cursor_pos--] = tmp;
				};
				break;
			case UI_TOP:
			case UI_PUP:
				while (cursor_pos) {
					char tmp;

					tmp = input[cursor_pos - 1];
					input[cursor_pos - 1] = '_';
					input[cursor_pos--] = tmp;
				};
				break;
			case UI_BOTTOM:
			case UI_PDN:
				while (cursor_pos < (strlen (input) - 1)) {
					char tmp;

					tmp = input[cursor_pos + 1];
					input[cursor_pos + 1] = '_';
					input[cursor_pos++] = tmp;
				};
				break;
			case UI_ESCAPE:
				strcpy (&input[0], data_backup);
				pos->data = &input[0];
				cursor_pos = strlen (input);
				input[cursor_pos] = '_';
				input[cursor_pos + 1] = 0;
				stop = 1;
				break;
			case UI_UP:
			case UI_DOWN:
			case UI_ENTER:
				stop = 1;
				break;
			case UI_BACKSPACE:
			case UI_BACKSPACE2:
			case UI_BACKSPACE3:
				if (cursor_pos) {
					memmove (&input[cursor_pos], &input[cursor_pos + 1],
							 strlen (input) - cursor_pos);
					input[cursor_pos - 1] = '_';
					cursor_pos--;
				};
				break;
				/*ignored keypresses.. */
			case UI_INSERT:
				break;
			default:
				if (c > 31 && c < 255) {	/*  input for buffer */
					memmove (&input[cursor_pos + 1], &input[cursor_pos],
							 strlen (input) - cursor_pos + 1);
					input[cursor_pos] = c;

					cursor_pos++;
				} else {		/* undefined keycode */
					undefined_key ("edit", c);
				}
				break;


		};
	};

	/* position our cursor at the end */
	while (cursor_pos < (strlen (input) - 1)) {
		char tmp;

		tmp = input[cursor_pos + 1];
		input[cursor_pos + 1] = '_';
		input[cursor_pos++] = tmp;
	};
	input[cursor_pos] = 0;

	pos->data = data_backup;
	node_setdata (pos, input);
	stop = 0;
}



Node *npos;
int tstartlevel;

static void						/* this is the same function I use in file,
								   perhaps I could make it and populate function
								   in tree.c instead,.. */ import_node (int level, int flags, char *data)
{
	level = level + tstartlevel;

	while (nodes_left (npos) > level)
		npos = node_left (npos);

	if (nodes_left (npos) == level)
		npos = node_insert_down (npos);

	if (nodes_left (npos) < level)
		npos = node_insert_right (npos);

	node_setflags (npos, flags);
	node_setdata (npos, data);
}

void node_duplicate_tree (Node *source, Node *target)
{
	int level, flags, sstartlevel;
	char *data;

	tstartlevel = nodes_left (target);

	node_setflags (target, node_getflags (source));
	node_setdata (target, node_getdata (source));
	npos = target;

	if (node_right (source)) {
		source = node_right (source);
		sstartlevel = nodes_left (source);
		while ((source != 0) & (nodes_left (source) >= sstartlevel)) {
			level = nodes_left (source) - sstartlevel + 1;
			flags = node_getflags (source);
			data = node_getdata (source);
			import_node (level, flags, data);
			source = node_recurse (source);
		}
	}
}

void app_mark ()
{
	{
		int stop = 0;
		Node *marked = pos;

		pos = node_insert_down (pos);
		node_setdata (pos, "-=- destination -=-");
		input[0] = 0;
		while (!stop) {
			int c;

			ui_draw (pos, node_getdata(marked), UI_MODE_MARKED);
			c = ui_input ();
			switch (c) {
				case UI_DEBUG:
					ui_draw (pos, input, UI_MODE_DEBUG);
					getch ();
					break;
				case UI_UP:
					if (node_up (pos)) {
						node_swap (pos, node_up (pos));
						pos = node_up (pos);
						if (pos == marked)
							marked = node_down (pos);
					}
					break;
				case UI_DOWN:
					if (node_down (pos)) {
						node_swap (pos, node_down (pos));
						pos = node_down (pos);
						if (pos == marked)
							marked = node_up (pos);
					}
					break;
				case UI_LEFT:
					if (node_left (pos)) {
						Node *tnode = node_insert_down (node_left (pos));

						node_swap (tnode, pos);
						node_remove (pos);
						pos = tnode;
					};
					break;
				case UI_RIGHT:
					if (node_up (pos) && (node_up (pos) != marked)) {	/* only if there is a node above */
						if (node_right (node_up (pos))) {	/* if there is children */
							Node *tnode =

								node_insert_up (node_right (node_up (pos)));
							node_swap (tnode, pos);
							node_remove (pos);
							pos = tnode;
						} else {	/* if there are no children */
							Node *tnode = node_insert_right (node_up (pos));

							node_swap (tnode, pos);
							node_remove (pos);
							pos = tnode;
						}
					}
					break;
				case ' ':
					node_swap (marked, pos);
					node_remove (marked);
					stop = 1;
					break;

					/* clone?????,.. this is the place to add symlinks,.. but how to implement it in the structure? */

				case UI_ENTER:
				case 'c':
				case 'C':		/*copy */
					node_duplicate_tree (marked, pos);
					stop = 1;
					break;
				case UI_ESCAPE:
				case 'q':
				case 'Q':
					stop = 1;
					node_remove (pos);
					pos = marked;
					break;
				default:
					undefined_key ("mark", c);
					break;
			};
		};
		stop = 0;
	};
}

char db_file[100];

int /* queries user wether to quit or not, returns 1 when quitting */
app_quit ()
{
	int c;

	ui_draw (pos, input, UI_MODE_QUIT);
	c = ui_input ();
	switch (c) {
		case 'y':
		case 'Y':
		case 'x':
		case 'X':
		case UI_QUIT:
			if (db_file[0] != (char) 255) {
				ascii_export ((Node *) node_root (pos), db_file);
				infof (" wrote stuff to '%s'", db_file);
			}
			return (1);
			break;
		case 'q':
		case 'Q':
			info (" quitting without saving.");
			return (1);
		case 's':
		case 'S':
			if (db_file[0] != (char) 255) {
				ascii_export ((Node *) node_root (pos), db_file);
				infof (" wrote stuff to '%s'", db_file);
			}
			return (0);
			break;
		case 'C':
		case 'c':
		case UI_ESCAPE:
			return (0);
		default:
			undefined_key ("quit", c);
			return (0);
	};
}

void app_search ()
{
	char query[100] = "Find: ";
	Node *query_start = pos;
	int query_start_level = nodes_left (pos);

	ui_draw (pos, (char *) query, UI_MODE_GETSTR);	/* query user for search term */

	pos = node_recursive_match ((char *) query, pos);

	if (pos == 0) {
		infof (" search for '%s' returned emptyhanded", query);
		pos = query_start;
		return;
	};

	while (pos != 0 && (nodes_left (pos) >= query_start_level)) {
		int c;

		ui_draw (pos, (char *) query, UI_MODE_SEARCH);
		c = ui_input ();
		switch (c) {
			case 's':
			case 'S':
			case UI_ENTER:
			{
				return;
			}
				break;
			case 'c':
			case 'C':
			case 'q':
			case 'Q':
			case UI_ESCAPE:
			{
				pos = query_start;
				return;
			}
				break;
			case 'n':
			case 'N':
			case ' ':
				pos = node_recursive_match ((char *) query, pos);
				break;
			default:
				undefined_key ("search", c);
				break;
		};
	};
	info (" end of search");
	pos = query_start;
}

void app_remove ()
{
	if (node_right (pos)) {
		int c;

		ui_draw (pos, "node has children, really remove?", UI_MODE_CONFIRM);
		c = ui_input ();
		if ((c == 'y') || (c == 'Y'))
			pos = node_remove (pos);

	} else {
		pos = node_remove (pos);
	};
}

void app_export ()
{
	int c, stop = 0;
	char filename[100];

	while (!stop) {
		ui_draw (pos, "", UI_MODE_EXPORT);
		c = ui_input ();
		switch (c) {
			case '?':
				strcpy ((char *) filename, "Save help-include file in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					help_export (node_top (pos), filename);
				stop = 1;
				break;
			case 'h':
			case 'H':
				strcpy ((char *) filename, "Save output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					html_export (node_top (pos), filename);
				stop = 1;
				break;
			case 'a':
			case 'A':
				strcpy ((char *) filename, "Save output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					ascii_export (node_top (pos), filename);
				stop = 1;
				break;
			case 'l':
			case 'L':
				strcpy ((char *) filename, "Save output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (strlen (filename))
					latex_export (node_top (pos), filename);
				stop = 1;
				break;
			case 'p':
			case 'P':
				strcpy ((char *) filename, "Save output in:");
				ui_draw (pos, (char *) filename, UI_MODE_GETSTR);
				if (!strlen (filename))
					return;
				latex_export (node_top (pos), "hnb.tmp.2.tex");
				{
					char cmd_buf[400];

					sprintf (cmd_buf, "latex hnb.tmp.2.tex &&\
dvips hnb.tmp.2.dvi -o %s &&\
rm hnb.tmp.*", filename);
					system (cmd_buf);
				}
				ui_end ();
				ui_init ();
				stop = 1;
				break;
			case 'c':
			case 'C':
			case UI_ESCAPE:
				stop = 1;
				break;
			default:
				undefined_key ("export", c);
		};
	}
}


void app_navigate ()
{
	int help_level = 1;
	int stop = 0;

	while (!stop) {
		int c;

		ui_draw (pos, input, UI_MODE_HELP0 + help_level);
		c = ui_input ();
		switch (c) {
			case UI_DEBUG:
				ui_draw (pos, input, UI_MODE_DEBUG);
				getch ();
				break;
			case UI_EXPORT:
				app_export ();
				break;
			case UI_QUIT:
				stop = app_quit ();
				break;
			case UI_FIND:
				app_search ();
				break;
			case UI_MARK:
				app_mark ();
				break;
			case UI_IMPORT:{
				char filename[100];

				strcpy ((char *) filename, "File to import:");
				ui_draw (pos, (char *) filename, UI_MODE_IMPORT);
				if (strlen (filename))
					pos = ascii_import (pos, filename);
			}
				break;

			case UI_SAVE:
				if (db_file[0] != (char) 255) {
					ascii_export ((Node *) node_root (pos), db_file);
					infof (" wrote stuff to '%s'", db_file);
				}
				break;

			case UI_HELP:
				help_level++;
				if (help_level >= 3)
					help_level = 0;
				break;

			case UI_TOGGLE_TODO:
				if (node_getflags (pos) & F_todo) {
					node_setflags (pos, node_getflags (pos) - F_todo);
				} else {
					node_setflags (pos, node_getflags (pos) + F_todo);
				}
				node_update_parents_todo (pos);
				break;
			case UI_TOGGLE_DONE:
				if (node_getflags (pos) & F_done) {
					node_setflags (pos, node_getflags (pos) - F_done);
				} else {
					node_setflags (pos, node_getflags (pos) + F_done);
				};
				node_update_parents_todo (pos);
				break;
			case UI_SORT:
				pos = node_sort_siblings (pos);
				break;
			case UI_LOWER:
				if (node_left (pos)) {
					Node *tnode;

					while (node_down (pos)) {
						tnode = node_insert_down (node_left (pos));
						node_swap (tnode, node_bottom (pos));
						node_remove (node_bottom (pos));
					};
					tnode = node_insert_down (node_left (pos));
					node_swap (tnode, node_bottom (pos));
					node_remove (node_bottom (pos));
					pos = tnode;
				};
				break;
			case UI_RAISE:
				if (node_up (pos)) {
					Node *tnode, *first_moved;

					pos = node_up (pos);	/* go up  */

					if (!(tnode = node_right (pos))) {	/* must install a child */
						tnode = node_insert_right (pos);
					} else {
						tnode = node_insert_down (node_bottom (tnode));
					}
					node_swap (node_down (pos), tnode);
					node_remove (node_down (pos));
					first_moved = tnode;

					while (node_down (pos)) {
						tnode =
							node_insert_down (node_bottom (node_right (pos)));
						node_swap (node_down (pos), tnode);
						node_remove (node_down (pos));
					};
					pos = first_moved;
				};
				break;
			case UI_TOP:
				if (node_getflags (pos) & F_temp)
					pos = node_remove (pos);
				input[0] = 0;
				pos = node_root (pos);
				break;
			case UI_UP:
				if (node_getflags (pos) & F_temp) {
					pos = node_remove (pos);
				} else {
					if (node_up (pos))
						pos = node_up (pos);
				};
				input[0] = 0;			
				break;
			case UI_DOWN:
				if (node_getflags (pos) & F_temp) {
					pos = node_remove (pos);
				} else if (node_down (pos))
					pos = node_down (pos);
				input[0] = 0;
				break;
			case UI_PDN:
				input[0] = 0;
				{
					int n;

					for (n = 0; n < 10; n++)
						if (node_down (pos))
							pos = node_down (pos);
				};				/* kan ikke få pg_up/pg_dn mengde fra ui,.. (ikke nå iallefall,..)
								   modul skillet er for godt definert */
				break;
			case UI_PUP:
				if (node_getflags (pos) & F_temp)
					pos = node_remove (pos);
				input[0] = 0;
				{
					int n;

					for (n = 0; n < 10; n++)
						if (node_up (pos))
							pos = node_up (pos);
				};
				break;
			case UI_LEFT:
				if (node_getflags (pos) & F_temp) {
					pos = node_remove (pos);
				} else {
					if (node_left (pos))
						pos = node_left (pos);
				};
				input[0] = 0;
				break;
			case UI_RIGHT:
				if (node_right (pos)) {
					pos = node_right (pos);
				} else {				
						if(node_getdata(pos)[0]){
						node_insert_right(pos);

						if(node_getflags (pos) & F_temp)
							node_setflags(pos,node_getflags(pos)-F_temp);
						node_setflags(pos->right,node_getflags(pos)+F_temp);

						pos=node_right(pos);
						node_update_parents_todo (pos);
					}
				};
				input[0] = 0;
				break;
			case UI_REMOVE:
				app_remove ();
				node_update_parents_todo (pos);
				break;
			case UI_COMPLETE:
				if (strcmp (input, node_getdata (pos)) == 0) {
					if (node_right (pos)) {
						pos = node_right (pos);
					} else {
						if(node_getdata(pos)[0]){
							node_insert_right(pos);

							if(node_getflags (pos) & F_temp)
								node_setflags(pos,node_getflags(pos)-F_temp);
							node_setflags(pos->right,node_getflags(pos)+F_temp);

							pos=node_right(pos);
							node_update_parents_todo (pos);
						}

						
					}
					input[0] = 0;
				} else {
					strcpy (input, node_getdata (pos));
				}
				break;
			case UI_ESCAPE:
				if (node_getflags (pos) & F_temp) {
					pos = node_remove (pos);
				} else {
					stop = app_quit ();
					/* other purpose for escape?? quit perhaps? */
				};
				input[0] = 0;
				break;
			case UI_ENTER:
				if (!strlen (input)) {
					app_edit ();
				} else {
					if (node_getflags (pos) & F_temp) {
						node_setflags (pos, node_getflags (pos) - F_temp);
					} else {
						pos = node_insert_down (node_bottom (pos));
						node_setdata (pos, input);
						if (node_getflags (node_left (pos)) & F_todo) {
							node_setflags (pos, F_todo);
							node_update_parents_todo (pos);
						}

					};
					input[0] = 0;
				};
				break;
			case UI_BACKSPACE:
			case UI_BACKSPACE2:
			case UI_BACKSPACE3:
				if (!strlen (input)) {
					app_remove ();
				} else {
					input[strlen (input) - 1] = 0;
					if (node_getflags (pos) & F_temp)
						if (node_up (pos))
							pos = node_remove (pos);
				};
				break;
			case UI_INSERT:
				pos = node_insert_down (pos);
				if (node_left (pos))
					if (node_getflags (node_left (pos)) & F_todo)
						node_setflags (pos, F_todo);
				node_update_parents_todo (pos);

				app_edit ();
				break;

			default:
				if (c > 31 && c < 255) {	/*  input for buffer */
					input[strlen (input) + 1] = 0;
					input[strlen (input)] = c;
				} else
					undefined_key ("navigation", c);

				break;
		};

		if (strlen (input)) {
			if (node_getflags (pos) & F_temp) {
				node_setdata (pos, input);
			} else {
				if (node_match (input, pos)) {
					if (strcmp
						(node_getdata (node_match (input, pos)),
						 node_getdata (pos)) != 0)
						pos = node_match (input, pos);
				} else {
					pos = node_insert_down (node_bottom (pos));
					node_setflags (pos, F_temp);
					node_setdata (pos, input);
					if (node_left (pos))
						if (node_getflags (node_left (pos)) & F_todo)
							node_setflags (pos, F_todo + F_temp);
					node_update_parents_todo (pos);

				};
			};
		};

	};
}

int main (int argc, char **argv)
{
	int i;
	int def_db = 0;
	char *ui;

	db_file[0] = 0;

	for (i = 1; i < argc; i++) {
		const char *sw = argv[i];

		if (sw[0] == '-' && sw[1] == '-')
			sw++;
		if (!strcmp (sw, "-h") || !strcmp (argv[i], "--help")) {
			usage (argv[0]);
			exit (0);
		} else if (!strcmp (sw, "-v") || !strcmp (argv[i], "--version")) {
			printf ("%s\n", VERSION);
			exit (0);
		} else if (!strcmp (sw, "-t") || !strcmp (argv[i], "--tutorial")) {
			db_file[0] = (char) 255;
		} else if (!strcmp (sw, "-ui")) {
			ui = argv[++i];
		} else {
			strcpy (&db_file[0], sw);
		}
	}

	if (db_file[0] == 0) {
#ifndef WIN32
		sprintf (db_file, "%s/.hnb", getenv ("HOME"));
#endif
#ifdef WIN32
		sprintf (db_file, "C:\\hnb_data");
#endif
		def_db = 1;
	};

	input[0] = 0;
	pos = tree_new ();

	if (db_file[0] == (char) 255)
		pos = help_import (pos);
	else
		pos = ascii_import (pos, db_file);

	if ((!node_right (pos)) && (!node_down (pos)) && (!node_up (pos))
		&& def_db) pos = help_import (pos);


	ui_init ();

	app_navigate ();

	ui_end ();
	return (0);
}
