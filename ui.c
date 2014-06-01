#include "node.h"
#include "tree.h"
#include "curses.h"
#include "version.h"
#define UI_C
#include "ui.h"

int nodes_above;
int middle_line;
int nodes_below;

void
ui_init ()
{
  initscr ();
  clear ();
  keypad (stdscr, TRUE);
  nonl ();
  cbreak ();
  noecho ();
  middle_line = LINES / 3;
  /* COLS ? */
  nodes_above = middle_line;
  nodes_below = (LINES *2 / 3)-1; 
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

Node *
up (Node * node)
{
  if (node_up (node))
    return (node_up (node));
  else
    return (node_left (node));
}

Node *
down (Node * node)
{
  if (node_down (node))
    {
      return (node_down (node));
    }
  else
    {
/*		while( !node_down(node_left(node)) | node_left(node)  )
			node=node_left(node);*/
      while (node != 0)
	{
	  node = node_left (node);
	  if (node_down (node))
	    return (node_down (node));
	};

    }
  return (0);
}

void help_draw(int mode, char *message){
int pos;
int line;
#define i(KEY,TEXT) {move(line, pos * (COLS/6)  );attrset(A_REVERSE);addstr(KEY);attrset(A_NORMAL);addstr(" ");addstr(TEXT);pos++;}

  switch(mode){
  	case UI_MODE_CONFIRM:
		line=LINES-2;pos=0;i(message,"");
		line=LINES-1;pos=0;i("yY","yes");i("other","no");
		break;
	case UI_MODE_NAVIGATE:
		line=LINES-2;pos=0;
		i("return","edit");
		i("abc..","entry/search");
		i("tab","complete");
		
		i("arrows","navigate");
		

		line=LINES-1;pos=0;

		i("^space","move");
		i("del","remove");
		i("ins","insert");

		
		i("Esc,F1","menu");


		break;
	case UI_MODE_EDIT:
		line=LINES-2;pos=0;
		i("return","confirm");
		i("abc..","entry");
		i("esc","cancel");
		i("left/right","move");
		break;
	case UI_MODE_QUIT:
		line=LINES-2;pos=0;	
		i("Really QUIT?","");
		line=LINES-1;pos=0;	
		i("Y","save/quit");
		i("Q","don't save/quit");
		i("S","save/don't quit");
		i("Esc","cancel");
		break;
	case UI_MODE_MENU:
		line=LINES-2;pos=0;	
		i("SD","save");
		i("Q","quit");
		i("O","sort");
		i("hnb",VERSION);
		line=LINES-1;pos=0;	
		i("Esc","cancel");		
		i("X","save/quit");
		i("","");
		i("   ","(c) Øyvind Kolås 2000/2001");
		break;
	case UI_MODE_MARKED:
		line=LINES-2;pos=0;	
		line=LINES-1;pos=0;	
		i("arrows","move");
		i("space","drop");
		i("Esc","cancel");		
		break;
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

int draw_node(int line_start, int col_start, char *data, int draw_mode){
    int lines_used=0;
	int col_end= COLS; /* -col_start-1; */
	move(line_start, col_start);

	if ( draw_mode & D_M_CHILD ) col_end -= 3;  /* need space to show we've got a subnode*/
	
	if( draw_mode & D_M_WRAP ){
		 int pos, col=col_start;
		 char word[100];
		 word[0]=0;	 

		 for(pos=0;pos<=strlen(data);pos++)
		    switch(data[pos]){
				case 0:
				case ' ':
					if(col+strlen(word)+1 >= col_end){
						col=col_start;
						lines_used++;
					};
						move(line_start+lines_used,col);
						if( ! (draw_mode & D_M_TEST)){
							if ( line_start + lines_used >= 0 ){
								addstr(word);
								if(data[pos]!= 0) addch(' ');
							};
						};
						col += strlen(word)+1;
						word[0]=0;
					break;
				default:
					if(strlen(word)<98){
						word[strlen(word)+1]=0;
						word[strlen(word)]=data[pos];
					};
					break;
			};
	} else if( ! (draw_mode & D_M_TEST)) addnstr(data, col_end-col_start );
			
	if( ! (draw_mode & D_M_TEST)) if ( draw_mode & D_M_CHILD ) addstr(" ..");
	
	lines_used++;
	
	return lines_used;
}

void
ui_draw (Node * node, char *input, int mode)
{
  int lines;
  erase ();

/*FIXME?: calculate .. startlevel,.. making the interface move right if the 
  	nodes are really nested */

{ 							/* draw nodes above selected node */
  int line = middle_line;

  Node *tnode = up (node);
  while (tnode != 0)
    {
      draw_node( line -=  draw_node(0,indentlevel(tnode), node_getdata(tnode), D_M_TEST+D_M_WRAP+(node_right(tnode)?D_M_CHILD:0))
	  	, indentlevel(tnode), node_getdata(tnode), D_M_WRAP+(node_right(tnode)?D_M_CHILD:0));
	
      tnode = up (tnode);
      if (middle_line - nodes_above >= line)
	tnode = 0;
    };
}

{						    /* draw the selected node.. */
  attrset (A_REVERSE);
  lines=draw_node(middle_line, indentlevel(node), node_getdata(node),D_M_WRAP+(node_right(node)?D_M_CHILD:0));
  attrset (A_BOLD);
  draw_node(middle_line, indentlevel(node), input,D_M_WRAP);
  attrset (A_NORMAL);  
}

{ /* draw lines below selected node */
   int line = middle_line+lines;
   Node *tnode = down (node);   
   
    /*	if(nodes_right(node))     // expand the current node,..
		tnode=node_right(node);	  // one level if possible
	else */

  while (tnode != 0)
    {
      line+=draw_node(line, indentlevel(tnode), node_getdata(tnode),D_M_WRAP+(node_right(tnode)?D_M_CHILD:0));
    
      tnode = down (tnode);
      if (middle_line + nodes_below <= line)
	tnode = 0;
    };
};

	help_draw(mode,input);
	
	move(LINES-1,COLS-1);

  refresh ();
}

void
ui_end ()
{
  clear ();
  refresh();
  endwin ();
}

int
ui_input ()
{
  int c;
  c = getch ();
  switch (c)
    {
    case KEY_UP:
      return (UI_UP);
      break;
    case KEY_DOWN:
      return (UI_DOWN);
      break;
    case KEY_LEFT:
      return (UI_LEFT);
      break;
    case KEY_RIGHT:
      return (UI_RIGHT);
      break;
    case KEY_HOME:
	case KEY_FIND:
      return (UI_TOP);  /* doesn't work in my rxvt,.. dunno why */
      break;
    case KEY_END:
	case KEY_SELECT:	
      return (UI_BOTTOM);
      break;
    case KEY_NPAGE:
      return (UI_PDN);
      break;
    case KEY_PPAGE:
      return (UI_PUP);
      break;
    case KEY_DC:
      return (UI_REMOVE);
      break;
    case KEY_RESIZE:
	  middle_line = LINES / 3;
	  nodes_above = middle_line;
	  nodes_below = (middle_line * 2) - 1;
      return(0);
      break;
    case 13:
      return (UI_ENTER);
      break;
	case KEY_F(10):
	case KEY_F(1):	
	case KEY_F(9):		
	case KEY_F(12):			
	case 11:/* ctrl+k */
	  return (UI_MENU);
	  break;
    case 27:
	case 5:
	case 24:/* ctrl+x */
      return (UI_ESCAPE);
      break;
    case 127:
    case KEY_BACKSPACE:
	case 8:
      return (UI_BACKSPACE);
      break;
    case 331: /* insert */
      return (UI_INSERT);
      break;
    case 0: /* ctrl+space */
	case 22: /* ctrl+v */
      return (UI_MARK);
    default:
      return (c);
      break;
    };
  return (0);
}
