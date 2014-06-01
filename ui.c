#include "node.h"
#include "tree.h"
#include "curses.h"
#include "version.h"
#include "stdio.h"
#include "prefs.h"
#define UI_C
#include "ui.h"

#ifdef NCURSES_MOUSE_VERSION
	#define MOUSE
#endif			
	#define hnbgimp
int nodes_above;
int middle_line;
int nodes_below;

#ifdef MOUSE
	#define mouse_maxnodes 100
	
	Node * mouse_node[mouse_maxnodes];
	int mouse_todo[mouse_maxnodes];
	int mouse_xstart[mouse_maxnodes];
	int mouse_xend[mouse_maxnodes];
	
	void mouse_clearnodes(){
	 	int j;
		for(j=0;j<mouse_maxnodes;j++){
			mouse_node[j]=(Node *)0;
			mouse_todo[j]=mouse_xstart[j]=mouse_xend[j]=0;
		}
	}
#endif

void ui_init (){
	initscr ();
	clear ();
	intrflush (stdscr, TRUE);
	keypad (stdscr, TRUE);
	nonl ();
	raw ();						/* enable the usage of ctl+c,ctrl+q,ctrl+z.. */
	noecho ();
	middle_line = LINES / 3;
	#ifdef MOUSE
		mousemask(BUTTON1_CLICKED+BUTTON3_CLICKED+BUTTON1_DOUBLE_CLICKED,NULL);
	#endif	
	/* COLS ? */
	nodes_above = middle_line;
	nodes_below = LINES - middle_line;
}

Node *up (Node *sel,Node *node){
	switch(prefs.collapse_mode){
		case COLLAPSE_ALL:
		case COLLAPSE_ALL_BUT_CHILD:
			if (node_up (node))
				return (node_up (node));
			else
				return (node_left (node));
			break;
		case COLLAPSE_NONE:
			return node_backrecurse(node);
			break;
		case COLLAPSE_ONLY_SIBLINGS:
			return node_up(node);
			break;
		case COLLAPSE_PATH:
			if(nodes_left(node)==nodes_left(sel))
				if(node_up(node))return(node_up(node));
			return(node_left(node));
			break;			
	}
	return 0;
}

Node *down (Node *sel,Node *node){
	switch(prefs.collapse_mode){
		case COLLAPSE_ALL_BUT_CHILD:
			if((node==sel) && (node_right(node)))
				return node_right(node); 
		case COLLAPSE_ALL:
			if (node_down (node)) {
				return (node_down (node));
			} else {
				while (node != 0) {
					node = node_left (node);
					if (node_down (node))
						return (node_down (node));
				};
			}
			break;
		case COLLAPSE_NONE:
			return node_recurse(node);
			break;
		case COLLAPSE_ONLY_SIBLINGS:
			return node_down(node);
			break;
		case COLLAPSE_PATH:
			return node_down(node);
			break;			
	}
	return (0);
}


/* this function draws a line at the top of the screen with various debug info,.. for tracing errors in the tree
   code, they seem to turn up every once in a while,.. but more and more seldomly
*/

void debug(Node *node,int mode){
	char buf[10];
	
	move(0,0);
	attron(A_REVERSE);addstr("id");attroff(A_REVERSE);sprintf(buf," %.4X ",(int)node&0xFFFF);addstr(buf);
	attron(A_REVERSE);addstr("up");attroff(A_REVERSE);sprintf(buf," %.4X ",(int)node->up&0xFFFF);addstr(buf);
	attron(A_REVERSE);addstr("dn");attroff(A_REVERSE);sprintf(buf," %.4X ",(int)node->down&0xFFFF);addstr(buf);			
	attron(A_REVERSE);addstr("<-");attroff(A_REVERSE);sprintf(buf," %.4X ",(int)node->left&0xFFFF);addstr(buf);
	attron(A_REVERSE);addstr("->");attroff(A_REVERSE);sprintf(buf," %.4X ",(int)node->right&0xFFFF);addstr(buf);			
	attron(A_REVERSE);addstr("lvl");attroff(A_REVERSE);sprintf(buf," %.2i ",nodes_left(node));addstr(buf);			
	attron(A_REVERSE);addstr("no");attroff(A_REVERSE);sprintf(buf," %.2i ",nodes_up(node));addstr(buf);			
	attron(A_REVERSE);addstr("flags");attroff(A_REVERSE);sprintf(buf," %i%i%i%i%i ",(node_getflags(node)>>4)&1,(node_getflags(node)>>3)&1,(node_getflags(node)>>2)&1,(node_getflags(node)>>1)&1,(node_getflags(node)>>0)&1);addstr(buf);		
	attron(A_REVERSE);addstr("mode");attroff(A_REVERSE);sprintf(buf," %.2i ",mode);addstr(buf);			
}

char toeleet(char in){
	char seta[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ(),0";
	char setb[]="aBCDeFGHiJKLMNoPQR$7uVWXyZ48(D3F6HiJKLMNoPQR57uVWXyZ[];o";
	int j;
	for(j=0;j<strlen(seta);j++){
		if(in==seta[j]){		
			return setb[j];
		}
	}
	return(in);
}

/*	help_draw is called with an mode, and an message that is used in some modes, see ui.h for the modes
	line - which line on the screen to draw on
	pos - which item number
	clr - clears the line
	clrc('a') - fills line with the character 'a'
	i("key","text") makes an entry at position specified by line and pos there are 6 positions per line
	rl("string") draws a string in reverse, spanning the whole line with the reverse box.
	mdc() clears all mouse mappings
	md(KEY_CODE) mouse define a keycode for the coords
*/

#ifdef MOUSE
	int mouse_codes[6][6];
	int mouse_warp=1;
	
	#define m_warp	{mouse_warp=1;}
	#define m_nwarp	{mouse_warp=0;}
	
	void mdc(){
		int pos,line;
		for(pos=0;pos<6;pos++)
			for(line=0;line<6;line++)
				mouse_codes[line][pos]=-1;
	}
	#define md(KEYCODE)	mouse_codes[(line-LINES)*-1][pos-1]=KEYCODE;
#endif

#ifndef MOUSE
	#define md(a)
	#define mdc()
	#define m_warp
	#define m_nwarp
#endif

void help_draw (int mode, char *message){
	int pos;
	int line;
	
	#define clrc(a) {int c;move(line,0);for(c=0;c<COLS;c++)addch(a);};
	#define clr	clrc(' ')
	#define rl(a) attrset(A_REVERSE);clr;move(line,0);addstr(a);attrset(A_NORMAL);
	#define i(KEY,TEXT) {move(line, pos * (COLS/6)  );attrset(A_REVERSE);addstr(KEY);attrset(A_NORMAL);addstr(TEXT);pos++;}
	
	switch (mode) {
		case UI_MODE_CONFIRM:mdc();m_nwarp;
			line = LINES - 2;
			rl(message);
			line = LINES - 1;
			pos = 0;
			clr;
			i (" Y ", " yes");md('y');
			i (" other", " no");md('n');
			break;
		case UI_MODE_HELP2:mdc();m_warp;
			break;
		case UI_MODE_HELP0:		
			line = LINES - 1;mdc();m_warp;
			pos = 0;
			clr;
			i (" F1 ", " help");md(UI_HELP);
			i ("arrows", " move");
			i ("return", " edit");md(UI_ENTER);
			i ("ins", " insert");md(UI_INSERT);
			i ("del", " remove");md(UI_REMOVE);
			i ("^X", " quit");md(UI_QUIT);
			break;
		case UI_MODE_HELP1:
			line = LINES - 5;m_warp;
			pos = 0;
			clr;
			i ("hnb", VERSION);mdc();
			i ("", "");
			i ("", "");
			i ("^F", " search");md(UI_FIND);
			i ("^O", " sort");md(UI_SORT);
			i ("^V", " viewmode");md(22);
			line = LINES - 4;
			pos = 0;
			clr;
			i ("(c)", " Øyvind Kolås 2000/2001");
			i ("", "");
			i ("", "");
			i ("", "");
			i ("", "");
			i ("^L", " import");md(UI_IMPORT);			
			line = LINES - 3;
			pos = 0;
			clr;
			i ("", "");
			i ("", "");
			#ifndef WIN32
				i ("^space", " grab");md(UI_MARK);
			#endif
			#ifdef WIN32
				i (" ^G ", " grab");
			#endif
			i ("^T", " todo toggle");md(UI_TOGGLE_TODO);
			i ("^D", " done toggle");md(UI_TOGGLE_DONE);
			i ("^E", " export");md(UI_EXPORT);
			line = LINES - 2;
			pos = 0;
			clr;
			i ("", "");
			i ("abc..", " match");
			i ("TAB", " complete");md(UI_COMPLETE);
			i ("^B", " parentify");md(UI_LOWER);
			i ("^N", " childify");md(UI_RAISE);
			i ("^S", " save");md(UI_SAVE);
			line = LINES - 1;
			pos = 0;
			clr;
			i ("F1", " help");md(UI_HELP);
			i ("arrows", " move");
			i ("return", " edit");md(UI_ENTER);
			i ("ins", " insert");md(UI_INSERT);
			i ("del", " remove");md(UI_REMOVE);
			i ("^X", " quit");md(UI_QUIT);
			break;
		case UI_MODE_EDIT:
			line = LINES - 2;mdc();m_nwarp;
			pos = 0;
			clr;
			i ("abc.. ", " entry");
			i ("", "");
			i (" esc  ", " cancel editing");md(UI_ESCAPE);
			i ("", "");
			i ("arrows", " move cursor");
			line = LINES - 1;
			pos = 0;
			clr;
			i ("return", " confirm");md(UI_ENTER);
			i ("", "");
			i ("bs/del", " remove char");md(UI_BACKSPACE);
			i ("", "");
			i (" home/end ", " warp cursor");md(UI_TOP);
			break;
		case UI_MODE_QUIT:
			line=LINES-3;
			rl("Really quit hnb?");
			line = LINES - 2;mdc();m_nwarp;
			pos = 2;
			clr;
			i (" Y,X ", " Save and Quit");md('Y');
			i ("", "");md('Y');
			i ("  Q  ", " Discard changes/Quit");md('Q');
			i ("", "");md('Q');
			line = LINES - 1;
			pos = 2;
			clr;
			i ("  S  ", " Save without quit");md('s');
			i ("", "");md('s');
			i ("Esc,C", " Cancel");md('c');
			i ("", "");md('c');
			break;
		case UI_MODE_EXPORT:m_nwarp;
			line = 0;mdc();
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
			rl("");
			line = LINES - 1;
			pos = 0;
			clr;
			i (" A ", " Ascii");md('a');
			i (" H ", " html");md('h');
			i (" L ", " Latex");md('l');
			i (" P ", " PostScript");md('p');
			i (" | ", " pipe");md('|');
			i ("Esc,C", " Cancel");md('c');
			line = LINES - 2;
			pos = 0;
			clr;
			rl ("Export menu select format");
	break;
		case UI_MODE_MARKED:
			line = LINES - 1;mdc();m_nwarp;
			pos = 0;
			clr;
			pos++;
			i ("arrows", " navigate");
			pos++;
			i ("space", " move");md(' ');
			i ("C Enter", " copy");md(UI_ENTER);
			i ("Esc,Q", " cancel");md('q');
			line = LINES - 2;
			pos = 0;
			clr;
			rl ("node grabbed, move destination");
			break;
		case UI_MODE_ERROR:
			mdc();
			line = 0;
			pos = 0;clr;
			i ("Error:", message);
			break;
		case UI_MODE_INFO:
			mdc();
			line = 0;
			pos = 0;clr;
			i ("Info:", message);
			break;
		case UI_MODE_GETSTR:
			line = LINES - 2;
			rl(message);
			line = LINES - 1;m_nwarp;
			pos = 1;
			clr;
			i ("", ">");
			break;
		case UI_MODE_IMPORT:
			line = 0;m_nwarp;
			pos = 0;
			clr;
			i ("      ",
			   " Enter the name of a file to be imported(inserted) into the");
			line = 1;
			pos = 0;
			clr;
			i (" info ", " tree at the current location, just press enter to cancel");
			line = 2;
			rl("");
			line = LINES - 2;
			rl(message);
			line = LINES - 1;			
			pos = 1;
			clr;
			i ("", ">");
			break;
		case UI_MODE_SEARCH:
			line = LINES - 2;mdc();m_nwarp;
			pos = 0;
			clr;
			rl ("Searching for:");
			attrset(A_REVERSE);
			addch (' ');
			addstr (message);
			attrset(A_NORMAL);			
			line = LINES - 1;
			pos = 3;clr;
			i ("Enter,S", " stop");md('s');
			i ("N,space", " next");md(' ');
			i ("Esc,C", " cancel");md('c');
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
#define D_M_EDIT	8

int hnb_edit_posup=0;
int hnb_edit_posdown=0;

int draw_item (int line_start, int col_start, char *data, int draw_mode, int completion){
	int lines_used = 0;
	int col_end = COLS;
	#define udc_no 0
	#define udc_yes 1
	#define udc_below 2
	#define udc_done 3
	int udc= udc_no;
	
	if(draw_mode&D_M_EDIT)udc=udc_yes;
	
	move (line_start, col_start);
	
	if (draw_mode & D_M_WRAP) {
		int pos, col = col_start;
		char word[200];			/* maximum displayed (not stored) word length */
		
		word[0] = 0;
		
		move (line_start, col);
		
	if (draw_mode&D_M_EDIT){
		col=col_start+=3;		
		} else 
		if (completion > -1) {/* todo bullets */
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

	if(udc==udc_yes){
		hnb_edit_posup=0;
		hnb_edit_posdown=strlen(data);
		}

	for (pos = 0; pos <= strlen (data); pos++)
		switch (data[pos]) {
			case 0:
				case ' ':
					if (col + strlen (word) + 1 >= col_end) {
					  if(udc==udc_yes){
					  	hnb_edit_posup= completion-(col-col_start);
					  } else if(udc==udc_below){
					  	hnb_edit_posdown= (completion)+(col-col_start);
						udc=udc_done;
					  }

	col = col_start;
		lines_used++;

	if (LINES <= lines_used + line_start)
		return lines_used + 1;	/* avoid drawing past the screen */
			};
				move (line_start + lines_used, col);
					if (!(draw_mode & D_M_TEST)) {
						if (line_start + lines_used >= 0) {							
							addstr (word);
							if(udc==udc_yes){
								if(completion==pos){
									attron(A_REVERSE);
									addch(' ');
									udc=udc_below;
									attroff(A_REVERSE);
								}
							}
							if (data[pos] != 0)
								addch (' ');
							if(udc==udc_yes){
								if(pos>completion){
									char ch=word[strlen(word)-(pos-completion)];
									attron(A_REVERSE);									
									move(line_start+lines_used,col-(pos-completion)+strlen(word));
									addch(ch);
									udc=udc_below;
									attroff(A_REVERSE);
								}
							}

	}
		}
			col += strlen (word) + 1;
				word[0] = 0;
					break;
				case 9:
					if ((strlen (word) < 198)) {
						word[strlen (word) + 1] = 0;
						word[strlen (word)] = ' ';
					}
					break;
				default:
					if ((strlen (word) < 198)) {
						word[strlen (word) + 1] = 0;
						word[strlen (word)] = prefs.eleet_mode?toeleet(data[pos]):data[pos];
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

extern int hnb_nodes_up;
extern int hnb_nodes_down;

void ui_draw (Node *node, char *input, int mode){
	static int ignore_next=0;
	int lines;
	
	/*FIXME?: calculate .. startlevel,.. making the interface move right if the  nodes are really nested */
	
	if(!ignore_next)
		if (/*mode != UI_MODE_QUIT &&
			mode != UI_MODE_CONFIRM &&*/
			mode != UI_MODE_ERROR &&
			mode != UI_MODE_INFO &&
			/*mode != UI_MODE_GETSTR &&
			mode != UI_MODE_EXPORT &&
			mode != UI_MODE_IMPORT && */mode != UI_MODE_DEBUG)
	/* draw the nodes */
	{
	hnb_nodes_up=0;
	hnb_nodes_down=0;
	
		erase ();		
#ifdef MOUSE
	mouse_clearnodes();
#endif
/* draw nodes above selected node */
	{
		int line = middle_line;
			Node *tnode = up (node,node);

	while (tnode) {
		draw_item (line -=
			   draw_item (0, indentlevel (tnode),
				  node_getdata (tnode),
					  D_M_TEST + D_M_WRAP, -1),
						   indentlevel (tnode), node_getdata (tnode),
						   D_M_WRAP + (node_right (tnode) ? D_M_CHILD : 0),
						   node_calc_complete (tnode));
#ifdef MOUSE						   
	if(line>0){
		mouse_node[line]=tnode;
			mouse_todo[line]=node_getflags(tnode)&F_todo?1:0;
				mouse_xstart[line]=indentlevel(tnode)+3;
				}
#endif
	tnode = up (node,tnode);
		if (middle_line - nodes_above >= line)
			tnode = 0;
				hnb_nodes_up++;
			}
		}

/* draw the selected node.. */

#ifdef MOUSE						   
	mouse_node[middle_line]=node;
		mouse_todo[middle_line]=node_getflags(node)&F_todo?1:0;
			mouse_xstart[middle_line]=indentlevel(node)+3;
#endif


	if(mode== UI_MODE_EDIT){
		/* bullet */
			draw_item (middle_line, indentlevel (node),
				   "",D_M_WRAP,node_calc_complete (node));

	lines =	draw_item (middle_line, indentlevel (node),
		   node_getdata (node),
			   D_M_WRAP + D_M_EDIT+ (node_right (node) ? D_M_CHILD : 0),
				   (int)input);
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
		Node *tnode = down (node,node);

	lines += middle_line;

	while (tnode) {
#ifdef MOUSE						   
	mouse_node[lines]=tnode;
		mouse_todo[lines]=node_getflags(tnode)&F_todo?1:0;
			mouse_xstart[lines]=indentlevel(tnode)+3;
#endif
	lines += draw_item (lines,
		indentlevel (tnode),
			node_getdata (tnode),
				D_M_WRAP +
					(node_right (tnode) ? D_M_CHILD : 0),
						node_calc_complete (tnode));
				tnode = down (node,tnode);
				if (middle_line + nodes_below <= lines)
					tnode = 0;
				hnb_nodes_down++;
			};
		}

#ifdef MOUSE						   
{int j;/*fill in the gaps in the lines */
	for(j=1;j<lines;j++){
		if(mouse_node[j]==0){
			if(mouse_node[j-1]!=0){
				mouse_node[j]=mouse_node[j-1];
				mouse_xstart[j]=mouse_xstart[j-1];
			}
		}
	}
}
#endif
	
		
		
	}


	if(!ignore_next)help_draw (mode, input);

	if (mode == UI_MODE_GETSTR || mode == UI_MODE_IMPORT) {
		echo ();
		getstr (&input[0]);
		noecho ();
	};
	
	if(prefs.view_debug)
		debug(node,mode);
		
	move (LINES - 1, COLS - 1);	

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

extern Node *pos;
#ifdef MOUSE

#endif

int ui_input ()
{
#ifdef MOUSE
	MEVENT mouse;
#endif

	int c;

	c = getch ();
	switch (c) {
		case 22:
			prefs.collapse_mode++;
			if(prefs.collapse_mode>COLLAPSE_END)prefs.collapse_mode=0;
			ui_draw (pos, "", UI_MODE_HELP0 + prefs.help_level);			
			ui_draw(pos," changed collapse mode",UI_MODE_INFO);
			return UI_IGNORE;
			break;
		case KEY_F(10):
			prefs.eleet_mode= !prefs.eleet_mode;
			return UI_IGNORE;
			break;
		case UI_DEBUG:
			prefs.view_debug= !prefs.view_debug;
			return UI_IGNORE;
			break;
		case KEY_RESIZE:
			middle_line = LINES / 3;
			nodes_above = middle_line;
			nodes_below = LINES - middle_line;
			c = getch ();
			return UI_IGNORE;
			break;
#ifdef MOUSE			
	case KEY_MOUSE:
		getmouse(&mouse);
		if(mouse.bstate&BUTTON1_CLICKED || mouse.bstate&BUTTON1_DOUBLE_CLICKED){/*button1*/
			if (mouse.y >= LINES-5){
				int line=(mouse.y-LINES)*-1;
				int pos= (mouse.x/ (COLS/6));

	if( mouse_codes[line][pos]!= -1 ) return mouse_codes[line][pos];
		}

	if(mouse_warp){
		  if (mouse_node[mouse.y]!=0){
			if(mouse.x<mouse_xstart[mouse.y]-4){
				return(UI_LEFT);
				}
				if((mouse_todo[mouse.y]) && (mouse.x<mouse_xstart[mouse.y])){
					Node *tnode=mouse_node[mouse.y];

	if (node_getflags (tnode) & F_done) {
		node_setflags (tnode, node_getflags (tnode) - F_done);
			} else {
				node_setflags (tnode, node_getflags (tnode) + F_done);
					}

	node_update_parents_todo(tnode);
		return(UI_IGNORE);
			}
				if(mouse.x<mouse_xstart[mouse.y])
					return(UI_LEFT);
		
			if(mouse.x>=mouse_xstart[mouse.y]){
				if(mouse.y==middle_line){
					if(node_right(pos))pos=node_right(pos);
					} else {					
						pos=mouse_node[mouse.y];
						if(mouse.bstate&BUTTON1_DOUBLE_CLICKED)
							if(node_right(pos))pos=node_right(pos);
					}
				}
				return UI_IGNORE;
			  }

	  if( mouse.y > middle_line ) return UI_DOWN;
		  if( mouse.y < middle_line ) return UI_UP;			  
			  
			}
		}
		if(mouse.bstate&BUTTON3_CLICKED){/* button 3 */
			return(UI_HELP);
		}


	return UI_IGNORE;
		break;
#endif
	#ifdef hnbgimp
		case KEY_F (12):{ int c = 0; int x = 5, y = 5, i = 1;erase (); help_draw (11111, ""); draw (2, 2); refresh (); while (c != 27) { c = getch (); switch (c) { case 8: case KEY_UP: y--; break; case 2: case KEY_DOWN: y++; break; case 4: case KEY_LEFT: x--; break; case 6: case KEY_RIGHT: x++;break; case 'i': case 'I':{ int x, y;for (y = 31; y >= 0; y--) for (x = 63; x >= 0; x--) { pset (x, y, !pget (x, y)); draw (2, 2); refresh ();} }; case 'x': case 'X': i = !i; break; case 'c': cls (); break; case 'q': case KEY_F (12): c = 27; break; } x = x > 63 ?0 : x < 0 ? 63 : x; y = y > 31 ? 0 : y < 0 ? 31 : y; pset (x, y, i); pset (x, y, !pget (x, y)); draw (2, 2); refresh ();pset (x, y, !pget (x, y)); } return UI_IGNORE; }
			#endif
	}

	return (c);
}
