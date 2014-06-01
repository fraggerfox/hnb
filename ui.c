#include "node.h"
#include "tree.h"
#include "curses.h"
#include "version.h"
#include "stdio.h"
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
  intrflush(stdscr,TRUE);
  keypad (stdscr, TRUE);
  nonl ();
/*  cbreak ();*/
  raw();     /* enable the usage of ctl+c,ctrl+q,ctrl+z..*/
  noecho ();
  middle_line = LINES / 3;
  /* COLS ? */
  nodes_above = middle_line;
  nodes_below = LINES-middle_line; 
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
#define clr {int c;move(line,0);for(c=0;c<COLS;c++)addch(' ');};
#define clrc(a) {int c;move(line,0);for(c=0;c<COLS;c++)addch(a);};
#define i(KEY,TEXT) {move(line, pos * (COLS/6)  );attrset(A_REVERSE);addstr(KEY);attrset(A_NORMAL);addstr(TEXT);pos++;}

  switch(mode){
  	case UI_MODE_CONFIRM:
		line=LINES-1;pos=0;clr;i(message,"");
		line=LINES-2;pos=0;clr;i("","");i("","");i("","");i(" Y "," yes");i(" other"," no");
		break;
	case UI_MODE_HELP0:
		break;
	case UI_MODE_HELP1:
		line=LINES-1;pos=0;clr;
		i(" F1 "," help");
		i("arrows"," move");
		i("return"," edit");
		i("ins"," insert");
		i("del"," remove");
		i("^X"," quit");
		break;
	case UI_MODE_HELP2:
		line=LINES-5;pos=0;clr;
		i("hnb",VERSION);
		i("","");
		i("","");
		i("","");		
		i("^E"," export");		
		i("^L"," import");

		line=LINES-4;pos=0;clr;
		i("(c)"," Øyvind Kolås 2000/2001");
		i("","");
		i("","");
		i("^F"," search");		
		i("^B"," parentify");
		i("^N"," childify");		
		line=LINES-3;pos=0;clr;
		i("","");
		i("","");
		i("","");
		i("^O"," sort");		
		i("^T"," todo toggle");
		i("^D"," done toggle");		

		line=LINES-2;pos=0;clr;		

		i("  F1  "," help");
		i("arrows"," move");
		i("ins"," insert");
		i("^X"," quit");
		i("abc.."," match");
		line=LINES-1;pos=0;clr;

		i("return"," edit");
		i(" ^S "," save");		
		i("del"," remove");

		i("^space"," move");
		i(" TAB "," match completion");
		break;
	case UI_MODE_EDIT:
		line=LINES-2;pos=0;clr;
		i("abc.. "," entry");
		i("","");
		i(" esc  "," cancel editing");
		i("","");				
		i("left/right"," move cursor");
		line=LINES-1;pos=0;clr;		
		i("return"," confirm");
		i("","");
		i("bs/del"," remove char");
		i("","");				
		i(" home/end "," warp cursor");

		
		break;
	case UI_MODE_QUIT:
		line=LINES-2;pos=0;clr;
		i("","");
		i("","");
		i(" Y,X "," Save and Quit");
		i("","");
		i("  Q  "," Discard changes/Quit");
		i("","");
		line=LINES-1;pos=0;clr;
		i(" QUIT menu ","");
		i("","");
		i("  S  "," Save without quit");
		i("","");		
		i("Esc,C"," Cancel");
		i("","");		
		break;
	case UI_MODE_EXPORT:
		line=0;pos=0;clr;i("      "," The exported data is kind of fragile, no escaping is done of html data.");
		line=1;pos=0;clr;i(" info "," postscript/latex doesn't allow deeper nesting of items than 4");
		line=2;pos=0;clr;i("      ","");		
		line=3;pos=0;clr;i("      "," the data from the current level and down are exported, not higher levels");		
		line=4;pos=0;clr;i("                          ","");		
				
		line=LINES-2;pos=0;clr;
		i("","");
		i("","");		
		i(" A "," Ascii");
		i(" H "," html");
		i(" L "," Latex");
		i(" P "," PostScript");		
		line=LINES-1;pos=0;clr;
		i("Export menu"," select format");
		i("","");
		i("","");		
		i("","");				
		i("","");				
		i("Esc,C"," Cancel");

		break;
	case UI_MODE_MARKED:
		line=LINES-2;pos=0;clr;
		i("","");i("","");i("","");
		i("arrows"," move");
		i("space"," drop");
		i("Esc,C"," cancel");
		line=LINES-1;pos=0;clr;			
		i("","node grabbed, move to destianation and drop node");
		break;
	case UI_MODE_ERROR:
	    line=LINES-1;pos=0;clr;
		i("Error:",message);
	    break;
	case UI_MODE_INFO:
	    line=LINES-1;pos=0;clr;
		i("Info:",message);
	    break;
	case UI_MODE_GETSTR:
	    line=LINES-1;pos=0;clr;
		i(message," ");		
	    break;
	case UI_MODE_IMPORT:
		line=0;pos=0;clr;i("      "," Enter the name of a file to be imported(inserted) into the tree at");
		line=1;pos=0;clr;i(" info "," the current location, just press enter to cancel");
		line=2;pos=0;clr;i("                      ","");		
	    line=LINES-1;pos=0;clr;
		i(message," ");		
	    break;		
	case UI_MODE_SEARCH:
		line=LINES-2;pos=0;clr;
		i("Searching for:","");
		addch(' ');addstr(message);
		i("","");i("","");
		i("Enter,S"," stop");
		i("N,space"," next");
		i("Esc,C"," cancel");
		line=LINES-1;pos=0;clr;			
		i("","");
		i("","");
		break;
  };
}

#define indentstart 1
#define indentspace 5

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

int draw_node(int line_start, int col_start, char *data, int draw_mode, int completion){
    int lines_used=0;
	int col_end= COLS; /* -col_start-1; */
	move(line_start, col_start);

	if ( draw_mode & D_M_CHILD ) col_end -= 3;  /* need space to show we've got a subnode*/
	
	if( draw_mode & D_M_WRAP ){
		 int pos, col=col_start;
		 char word[100];
		 word[0]=0;	 


	move(line_start,col);

  if(completion!=-1){						/* todo bullets */
	    switch(completion){
			case 0:
		     if( ! (draw_mode & D_M_TEST))	  addstr("[ ] ");
		  	  col=col_start+=4;			 
			 break;
			case 1000:
 		     if( ! (draw_mode & D_M_TEST))	  addstr("[X] ");
		  	  col=col_start+=4;
			 break;
			default:{char str[10];
		  	  col=col_start+=6;
			sprintf(str,"[%i%%]",completion/10);
 		     if( ! (draw_mode & D_M_TEST))	  addstr(str);
			}
			 break;
		}
  } else {
#ifndef WIN32
	 int a;short b;
     attr_get(&a,&b,NULL); /* the standard bullet */
	 attrset(0);  
#endif
	 if(data[0]) if( ! (draw_mode & D_M_TEST))  addstr("-");
	 col=col_start+=2;
#ifndef WIN32
	 attrset(a);
#endif	 
  }

		 for(pos=0;pos<=strlen(data);pos++)
		    switch(data[pos]){
				case 0:
				case ' ':
					if(col+strlen(word)+1 >= col_end){
						col=col_start;
						lines_used++;

			      if (LINES <= lines_used+line_start)
					return lines_used+1;  /* avoid overflow whilst
											line breaking*/
						
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
      draw_node( line -=  draw_node(0,indentlevel(tnode), node_getdata(tnode), D_M_TEST+D_M_WRAP+(node_right(tnode)?D_M_CHILD:0),node_calc_complete(tnode))
	  	, indentlevel(tnode), node_getdata(tnode), D_M_WRAP+(node_right(tnode)?D_M_CHILD:0),node_calc_complete(tnode));
	
      tnode = up (tnode);
      if (middle_line - nodes_above >= line)
	tnode = 0;
    };
}

{						    /* draw the selected node.. */
  attrset (A_REVERSE);
  lines=draw_node(middle_line, indentlevel(node), node_getdata(node),D_M_WRAP+(node_right(node)?D_M_CHILD:0),node_calc_complete(node));
  attrset (A_BOLD);
 
if(mode!=UI_MODE_INFO && mode!=UI_MODE_GETSTR && mode !=UI_MODE_IMPORT && mode != UI_MODE_SEARCH && mode != UI_MODE_CONFIRM && mode != UI_MODE_ERROR )
  draw_node(middle_line, indentlevel(node), input,D_M_WRAP,node_calc_complete(node));
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
      line+=draw_node(line, indentlevel(tnode), node_getdata(tnode),D_M_WRAP+(node_right(tnode)?D_M_CHILD:0),node_calc_complete(tnode));
    
      tnode = down (tnode);
      if (middle_line + nodes_below <= line)
	tnode = 0;
    };
};

	help_draw(mode,input);
	
	move(LINES-1,COLS-1);
	
  refresh ();

	if(mode==UI_MODE_GETSTR || mode==UI_MODE_IMPORT){
		move(LINES-1,strlen(input)+1);
		echo();
		getstr(&input[0]);
		noecho();
	};
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
  c=getch();
  switch(c){
  	case KEY_RESIZE:
    middle_line = LINES / 3;
    nodes_above = middle_line;
    nodes_below = LINES-middle_line;
	c=getch();
	
    return UI_IGNORE;
	break;	
  }
  
  return(c);
}
