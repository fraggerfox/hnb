#include "node.h"
#include "tree.h"
#include "curses.h"
#include "version.h"
#include "stdio.h"
#define UI_C
#include "ui.h"
																																					#define hnbgimp
int nodes_above;
int middle_line;
int nodes_below;

void ui_init ()
{
	initscr ();
	clear ();
	intrflush (stdscr, TRUE);
	keypad (stdscr, TRUE);
	nonl ();
	raw ();						/* enable the usage of ctl+c,ctrl+q,ctrl+z.. */
	noecho ();
	middle_line = LINES / 3;
	/* COLS ? */
	nodes_above = middle_line;
	nodes_below = LINES - middle_line;
}

/*
Node *up(Node *node){
	if(node_up(node))return(node_up(node));
	else return(0);
}

Node *down(Node *node){
	if(node_down(node))return(node_down(node));
	else return(0);
}
*/

Node *up (Node *node)
{
	if (node_up (node))
		return (node_up (node));
	else
		return (node_left (node));
}

Node *down (Node *node)
{
	if (node_down (node)) {
		return (node_down (node));
	} else {
		while (node != 0) {
			node = node_left (node);
			if (node_down (node))
				return (node_down (node));
		};

	}
	return (0);
}

/*
	help_draw is called with an mode, and an message that is used in some modes, see ui.h for the modes
	line - which line on the screen to draw on
	pos - which item number
	clr - clears the line
	clrc('a') - fills line with the character 'a'
	i("key","text") makes an entry at position specified by line and pos there are 6 positions per line
*/

void help_draw (int mode, char *message)
{
	int pos;
	int line;

#define clrc(a) {int c;move(line,0);for(c=0;c<COLS;c++)addch(a);};
#define clr	clrc(' ')
#define i(KEY,TEXT) {move(line, pos * (COLS/6)  );attrset(A_REVERSE);addstr(KEY);attrset(A_NORMAL);addstr(TEXT);pos++;}

	switch (mode) {
		case UI_MODE_CONFIRM:
			line = LINES - 1;
			pos = 0;
			clr;
			i (message, "");
			line = LINES - 2;
			pos = 0;
			clr;
			i ("", "");
			i ("", "");
			i ("", "");
			i (" Y ", " yes");
			i (" other", " no");
			break;
		case UI_MODE_HELP0:
			break;
		case UI_MODE_HELP1:
			line = LINES - 1;
			pos = 0;
			clr;
			i (" F1 ", " help");
			i ("arrows", " move");
			i ("return", " edit");
			i ("ins", " insert");
			i ("del", " remove");
			i ("^X", " quit");
			break;
		case UI_MODE_HELP2:
			line = LINES - 5;
			pos = 0;
			clr;
			i ("hnb", VERSION);
			i ("", "");
			i ("", "");
			i ("", "");
			i ("^E", " export");
			i ("^L", " import");

			line = LINES - 4;
			pos = 0;
			clr;
			i ("(c)", " Øyvind Kolås 2000/2001");
			i ("", "");
			i ("", "");
			i ("^F", " search");
			i ("^B", " parentify");
			i ("^N", " childify");
			line = LINES - 3;
			pos = 0;
			clr;
			i ("", "");
			i ("", "");
			i ("", "");
			i ("^O", " sort");
			i ("^T", " todo toggle");
			i ("^D", " done toggle");

			line = LINES - 2;
			pos = 0;
			clr;

			i ("  F1  ", " help");
			i ("arrows", " move");
			i ("ins", " insert");
			i ("^X", " quit");
			i ("abc..", " match");
			line = LINES - 1;
			pos = 0;
			clr;

			i ("return", " edit");
			i (" ^S ", " save");
			i ("del", " remove");

#ifndef WIN32
			i ("^space", " grab");
#endif
#ifdef WIN32
			i (" ^G ", " grab");
#endif
			i (" TAB ", " match completion");
			break;
		case UI_MODE_EDIT:
			line = LINES - 2;
			pos = 0;
			clr;
			i ("abc.. ", " entry");
			i ("", "");
			i (" esc  ", " cancel editing");
			i ("", "");
			i ("left/right", " move cursor");
			line = LINES - 1;
			pos = 0;
			clr;
			i ("return", " confirm");
			i ("", "");
			i ("bs/del", " remove char");
			i ("", "");
			i (" home/end ", " warp cursor");
			break;
		case UI_MODE_QUIT:
			line = LINES - 2;
			pos = 0;
			clr;
			i ("", "");
			i ("", "");
			i (" Y,X ", " Save and Quit");
			i ("", "");
			i ("  Q  ", " Discard changes/Quit");
			i ("", "");
			line = LINES - 1;
			pos = 0;
			clr;
			i (" QUIT menu ", "");
			i ("", "");
			i ("  S  ", " Save without quit");
			i ("", "");
			i ("Esc,C", " Cancel");
			i ("", "");
			break;
		case UI_MODE_EXPORT:
			line = 0;
			pos = 0;
			clr;
			i ("      ", " The exported data is kind of fragile, no escaping is done of html data.");
			line = 1;
			pos = 0;
			clr;
			i (" info ", " postscript/latex doesn't allow deeper nesting of items than 4");
			line = 2;
			pos = 0;
			clr;
			i ("      ", "");
			line = 3;
			pos = 0;
			clr;
			i ("      ", " the data from the current level and down are exported, not higher levels");
			line = 4;
			pos = 0;
			clr;
			i ("                          ", "");
			line = LINES - 2;
			pos = 0;
			clr;
			i ("", "");
			i ("", "");
			i (" A ", " Ascii");
			i (" H ", " html");
			i (" L ", " Latex");
			i (" P ", " PostScript");
			line = LINES - 1;
			pos = 0;
			clr;
			i ("Export menu", " select format");
			i ("", "");
			i ("", "");
			i ("", "");
			i ("", "");
			i ("Esc,C", " Cancel");

			break;
		case UI_MODE_MARKED:
			line = LINES - 2;
			pos = 0;
			clr;
			pos++;
			i ("arrows", " navigate");
			pos++;
			i ("space", " move");
			i ("C Enter", " copy");
			i ("Esc,Q", " cancel");
			line = LINES - 1;
			pos = 0;
			clr;
			i (message, " grabbed, move destination");
			break;
		case UI_MODE_ERROR:
			line = LINES - 3;
			pos = 0;clr;

			line = LINES - 2;
			pos = 0;clr;

			line = LINES - 1;
			pos = 0;clr;
			i ("Error:", message);
			break;
		case UI_MODE_INFO:
			line = LINES - 3;
			pos = 0;clr;

			line = LINES - 2;
			pos = 0;clr;

			line = LINES - 1;
			pos = 0;
			clr;
			i ("Info:", message);
			break;
		case UI_MODE_GETSTR:
			line = LINES - 1;
			pos = 0;
			clr;
			i (message, " ");
			break;
		case UI_MODE_IMPORT:
			line = 0;
			pos = 0;
			clr;
			i ("      ",
			   " Enter the name of a file to be imported(inserted) into the tree at");
			line = 1;
			pos = 0;
			clr;
			i (" info ", " the current location, just press enter to cancel");
			line = 2;
			pos = 0;
			clr;
			i ("                      ", "");
			line = LINES - 1;
			pos = 0;
			clr;
			i (message, " ");
			break;
		case UI_MODE_SEARCH:
			line = LINES - 2;
			pos = 0;
			clr;
			i ("Searching for:", "");
			addch (' ');
			addstr (message);
			i ("", "");
			i ("", "");
			i ("Enter,S", " stop");
			i ("N,space", " next");
			i ("Esc,C", " cancel");
			line = LINES - 1;
			pos = 0;
			clr;
			i ("", "");
			i ("", "");
			break;
																																						#ifdef hnbgimp
																																			case 11111: line = LINES - 2; pos = 0; clr; i (" hnb-gimp ", " ver -0.1"); addch (' '); addstr (message); i ("", ""); i("", ""); i ("ESC,Q", " quit"); i ("arrows", " draw"); i (" X ", " toggle color"); line = LINES - 1; pos = 0; clr; i("", ""); i ("", ""); i ("", ""); i (" I ", " invert image"); i ("", ""); i (" C ", " clear canvas"); break;
																																					#endif
	};
}

#define indentstart 1
#define indentspace 4

int startlevel = 0;

#define indentlevel(a)	(nodes_left(a)-startlevel)*indentspace+indentstart

/*	draws the user interface,
	inputs: selected node,
	        contents of input buffer
		mode (as defined in ui.h)
*/

#define D_M_CHILD	1
#define D_M_WRAP	2
#define D_M_TEST	4

int draw_item (int line_start, int col_start, char *data, int draw_mode,
			   int completion)
{
	int lines_used = 0;
	int col_end = COLS;			/* -col_start-1; */

	move (line_start, col_start);

/*	if ( draw_mode & D_M_CHILD ) col_end -= 3;*//* need space to show we've got a subnode */

	if (draw_mode & D_M_WRAP) {
		int pos, col = col_start;
		char word[200];			/* maximum displayed (not stored) word length */

		word[0] = 0;

		move (line_start, col);

		if (completion > -1) {	/* todo bullets */
			switch (completion) {
				case 0:
					if (col != 0)
						move (line_start, col - 1);
					if (!(draw_mode & D_M_TEST))
						addstr ("[ ] ");
					col = col_start += 3;
					break;
				case 1000:
					if (col != 0)
						move (line_start, col - 1);
					if (!(draw_mode & D_M_TEST))
						addstr ("[X] ");
					col = col_start += 3;
					break;
				default:{
					char str[10];

					sprintf (str, "%2i%% ", completion / 10);
					if (col != 0) {
						move (line_start, col - 1);
					} else {
						col_start += 1;
					};
					if (!(draw_mode & D_M_TEST))
						addstr (str);
					col = col_start += 3;
				}
					break;
			}
/*		if( ! (draw_mode & D_M_TEST))addch((draw_mode & D_M_CHILD)?'+':' ');*/
		} else {
/*	 if(completion==-1)if(data[0]) if( ! (draw_mode & D_M_TEST))  addstr((draw_mode & D_M_CHILD)?"   ":"   ");*/
			col = col_start += 3;
		}

		if (!(draw_mode & D_M_TEST) && (draw_mode & D_M_CHILD)) {
			attron (A_BOLD);
		}

		for (pos = 0; pos <= strlen (data); pos++)
			switch (data[pos]) {
				case 0:
				case ' ':
					if (col + strlen (word) + 1 >= col_end) {
						col = col_start;
						lines_used++;

						if (LINES <= lines_used + line_start)
							return lines_used + 1;	/* avoid drawing past the screen */
					};
					move (line_start + lines_used, col);
					if (!(draw_mode & D_M_TEST)) {
						if (line_start + lines_used >= 0) {
							addstr (word);
							if (data[pos] != 0)
								addch (' ');
						};
					};
					col += strlen (word) + 1;
					word[0] = 0;
					break;
				default:
					if ((strlen (word) < 198)) {
						word[strlen (word) + 1] = 0;
						word[strlen (word)] = data[pos];
					};
					if (strlen (word) > col_end - col_start) {
						/* wordbreak with hyphen,..? */
					}

					break;
			};
	} else if (!(draw_mode & D_M_TEST))
		addnstr (data, col_end - col_start);

/*	if( ! (draw_mode & D_M_TEST)) if ( draw_mode & D_M_CHILD ) addstr(" ..");*/

	if (!(draw_mode & D_M_TEST) && (draw_mode & D_M_CHILD)) {
		attrset (A_NORMAL);
	}

	lines_used++;

	return lines_used;
}

void ui_draw (Node *node, char *input, int mode)
{
	static int ignore_next=0;
	int lines;

/*FIXME?: calculate .. startlevel,.. making the interface move right if the 
  	nodes are really nested */

if(!ignore_next)
	if (mode != UI_MODE_QUIT &&
		mode != UI_MODE_CONFIRM &&
		mode != UI_MODE_ERROR &&
		mode != UI_MODE_INFO &&
		mode != UI_MODE_GETSTR &&
		mode != UI_MODE_EXPORT &&
		mode != UI_MODE_IMPORT && mode != UI_MODE_DEBUG)

/* draw the nodes */
	{
		erase ();
/* draw nodes above selected node */
		{
			int line = middle_line;
			Node *tnode = up (node);

			while (tnode) {
				draw_item (line -=
						   draw_item (0, indentlevel (tnode),
									  node_getdata (tnode),
									  D_M_TEST + D_M_WRAP, -1),
						   indentlevel (tnode), node_getdata (tnode),
						   D_M_WRAP + (node_right (tnode) ? D_M_CHILD : 0),
						   node_calc_complete (tnode));
				tnode = up (tnode);
				if (middle_line - nodes_above >= line)
					tnode = 0;
			}
		}

/* draw the selected node.. */
		if(mode== UI_MODE_EDIT){
			/* draw node normally */
			lines =	draw_item (middle_line, indentlevel (node),
						   node_getdata (node),
						   D_M_WRAP + (node_right (node) ? D_M_CHILD : 0),
						   node_calc_complete (node));
			
			attrset (A_REVERSE);		
			move(middle_line,indentlevel(node)+(int)input+3);
			addch(node_getdata(node)[(int)input]);
		} else

		{	/* bullet */
			draw_item (middle_line, indentlevel (node), "", D_M_WRAP,
					   node_calc_complete (node));
			attrset (A_REVERSE);
			/* the selected node */
			lines =
				draw_item (middle_line, indentlevel (node),
						   node_getdata (node),
						   D_M_WRAP + (node_right (node) ? D_M_CHILD : 0),
						   -2);
			attrset (A_BOLD);
			/* the search input */
			if (mode == UI_MODE_HELP0 || mode == UI_MODE_HELP1
				|| mode == UI_MODE_HELP2) draw_item (middle_line,
													 indentlevel (node),
													 input, D_M_WRAP, -2);
		}
		attrset (A_NORMAL);		

/* draw lines below selected node */
		{
			Node *tnode = down (node);

			lines += middle_line;

/*if(node_right(node))  tnode=node_right(node); *//*strange one more level collapse mode */

			while (tnode) {
				lines += draw_item (lines,
									indentlevel (tnode),
									node_getdata (tnode),
									D_M_WRAP +
									(node_right (tnode) ? D_M_CHILD : 0),
									node_calc_complete (tnode));
				tnode = down (tnode);
				if (middle_line + nodes_below <= lines)
					tnode = 0;
			};
		}
	}


	if(!ignore_next)help_draw (mode, input);

	move (LINES - 1, COLS - 1);



	if (mode == UI_MODE_GETSTR || mode == UI_MODE_IMPORT) {
		move (LINES - 1, strlen (input) + 1);
		echo ();
		getstr (&input[0]);
		noecho ();
	};
/* it causes compile time warnings so, I uncomment it when I need it */

/*	if (mode == UI_MODE_DEBUG) {
		int pos;
		char msg[80];
		int t;

#define ipf(a,b,c) sprintf(msg,b,c);i(a,msg);
		int line;

		line = 0; pos = 0; clr;
		i ("       ", "");ipf ("data:", "  %s", node_getdata (node));

		line = 1; pos = 0; clr;
		i (" debug ", ""); ipf ("flags:", " %i", node_getflags (node)); pos += 2; ipf ("/\\", " %i", ((t = (int) node_up (node)) & 4095));

		line = 2; pos = 0; clr;
		i ("       ", ""); ipf ("level:", " %i", nodes_left (node)); pos += 1; ipf ("<-", " %i", (t = ((int) node_left (node))) & 4095); ipf ("", "%i", ((int) node) & 4095); ipf ("->", " %i", (t = (int) node_right (node)) & 4095);

		line = 3; pos = 0; clr;
		i ("       ", ""); ipf ("input:", " %s", input); pos += 2; ipf ("\\/", " %i", (t = (int) node_down (node)) & 4095); pos += 2;

		line = 4; pos = 0; clr; i ("                                ", ""); line = LINES - 1; pos = 0; clr; i (" press any key ", "");

	};*/
	refresh ();
	ignore_next=0;
	if(mode==UI_MODE_INFO || mode==UI_MODE_ERROR)ignore_next=1;

}

void ui_end ()
{
	clear ();
	refresh ();
	endwin ();
}

																																							#ifdef hnbgimp
																																				#define pset(a,b,c) v[b][a]=c;
																																			#define pget(a,b) v[b][a];
																																				#define mva(a,b,c) mvaddch(a,b,c)
																																	#define cw 33
																																		#define ch 17
																																			char qixels[16] = " ,'L`/~Pcw\\b]d\\@"; int v[32][64];void cls () { int x, y; for (x = 0; x < 64; x++) for (y = 0; y < 32; y++) pset (x, y, 0); } void draw (int xp, int yp) { int x, y; for (x = 0; x < 16; x++) for (y = 0; y < 32; y++) { mva (x + yp + 1,y + xp + 1, qixels[v[x * 2][y * 2] * 2 + v[x * 2 + 1][y * 2] + v[x * 2][y * 2 + 1] * 4 + v[x * 2 + 1][y * 2 + 1] * 8]);} for (x =xp; x <= xp + cw; x++) { mva (yp, x, '_'); mva (yp + ch, x, '~'); }; for (y = yp; y <= yp + ch; y++) { mva (y, xp + cw, '|'); mva(y, xp, '|'); }; mva (yp, xp, '.'); mva (yp, xp + cw, '.'); mva (yp + ch, xp, '`'); mva (yp + ch, xp + cw, '\''); move (0, 0); }

																																									#endif
int ui_input ()
{

	int c;

	c = getch ();
	switch (c) {
		case KEY_RESIZE:
			middle_line = LINES / 3;
			nodes_above = middle_line;
			nodes_below = LINES - middle_line;
			c = getch ();
			return UI_IGNORE;
			break;
																																					#ifdef hnbgimp
																																							case KEY_F (12):{ int c = 0; int x = 5, y = 5, i = 1;erase (); help_draw (11111, ""); draw (2, 2); refresh (); while (c != 27) { c = getch (); switch (c) { case 8: case KEY_UP: y--; break; case 2: case KEY_DOWN: y++; break; case 4: case KEY_LEFT: x--; break; case 6: case KEY_RIGHT: x++;break; case 'i': case 'I':{ int x, y;for (y = 31; y >= 0; y--) for (x = 63; x >= 0; x--) { pset (x, y, !pget (x, y)); draw (2, 2); refresh ();} }; case 'x': case 'X': i = !i; break; case 'c': cls (); break; case 'q': case KEY_F (12): c = 27; break; } x = x > 63 ?0 : x < 0 ? 63 : x; y = y > 31 ? 0 : y < 0 ? 31 : y; pset (x, y, i); pset (x, y, !pget (x, y)); draw (2, 2); refresh ();pset (x, y, !pget (x, y)); } return UI_IGNORE; }
																																		#endif
	}

	return (c);
}
