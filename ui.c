#include "node.h"
#include "tree.h"
#include "curses.h"
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
  nodes_below = (middle_line * 2) - 1;
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


#define indentstart 2
#define indentspace 4

int startlevel = 0;

#define indentlevel(a)	(nodes_left(a)-startlevel)*indentspace+indentstart

void
ui_draw (Node * node, char *input, int mode)
{
  int line;
  Node *tnode;

  erase ();

/*	calculate .. startlevel=0;*/

  tnode = node;

  attrset (A_REVERSE);
  move (middle_line, indentlevel (node));

  addstr (node_getdata (tnode));
  if (node_right (tnode))
    addstr ("  ..");

  attrset (A_BOLD);
  move (middle_line, indentlevel (node));
  addstr (input);

  attrset (A_NORMAL);

  line = middle_line;

  tnode = up (node);
  while (tnode != 0)
    {
      move (--line, indentlevel (tnode));
      addstr (node_getdata (tnode));
      if (node_right (tnode))
	addstr ("  ..");
      tnode = up (tnode);
      if (middle_line - nodes_above >= line)
	tnode = 0;
    };

  line = middle_line;

/*	if(nodes_right(node))
		tnode=node_right(node);
	else */
  tnode = down (node);

  while (tnode != 0)
    {
      move (++line, indentlevel (tnode));
      addstr (node_getdata (tnode));
      if (node_right (tnode))
	addstr ("  ..");
      tnode = down (tnode);
      if (middle_line + nodes_below <= line)
	tnode = 0;
    };


#define space attrset(A_NORMAL);addstr(" ");attrset(A_REVERSE);

  move (LINES - 2, 1);
  if (mode == UI_MODE_NAVIGATE)
    {
      attrset (A_REVERSE);
      addstr ("arrows - navigate");
      space;
      addstr ("<-' edit");
      space;
      addstr ("[esc] quit");
      space;
      addstr ("ctrl+space mark");
      space;
      addstr ("F1 misc");	  
      attrset (A_NORMAL);
    };
  if (mode == UI_MODE_EDIT)
    {
      attrset (A_REVERSE);
      addstr ("arrows - move cursor");
      space;
      addstr ("[esc] cancel");
      space; 
      addstr ("<-' saves");
      attrset (A_NORMAL);
    };
  if (mode == UI_MODE_QUIT)
    {
      attrset (A_REVERSE);
      addstr ("QUIT?");
      attrset (A_NORMAL);
    };
  move (LINES - 1, 1);
  if (mode == UI_MODE_NAVIGATE)
    {
      attrset (A_REVERSE);
      addstr ("tab - complete/enter child");
      space;
      addstr ("abc.. - entry/search");
      space;
      addstr ("del - remove");
      attrset (A_NORMAL);
    };
  if (mode == UI_MODE_EDIT)
    {
      attrset (A_REVERSE);
      addstr ("abc.. entry");
      space;
      addstr ("backspace - erase");
      attrset (A_NORMAL);
    };
  if (mode == UI_MODE_MARKED)
    {
      attrset (A_REVERSE);
      addstr ("space - move");
      space;
      addstr ("return - make symlink");
      space;
      addstr ("[esc] cancel");
      attrset (A_NORMAL);
    };
  if (mode == UI_MODE_MENU)
    {
      attrset (A_REVERSE);
      addstr ("sS - sort");
      attrset (A_NORMAL);
    };
  if (mode == UI_MODE_QUIT)
    {
      attrset (A_REVERSE);
      addstr ("yY - quit and save");
      space;
      addstr ("qQ - quit no save");
      space;
      addstr ("sS - save no quit");
      space;
      addstr ("[esc] cancel");
      attrset (A_NORMAL);
    };


  refresh ();
}


void
ui_end ()
{
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
      return (UI_TOP);
      break;
    case KEY_END:
      return (UI_BOTTOM);
      break;
    case KEY_DC:
      return (UI_REMOVE);
      break;
    case 13:
      return (UI_ENTER);
      break;
	case KEY_F(1):
	  return (UI_MENU);
	  break;	  
    case 27:
      return (UI_ESCAPE);
      break;
    case 127:
    case KEY_BACKSPACE:
      return (UI_BACKSPACE);
      break;
    case 0:
      return (UI_MARK);
    default:
      return (c);
      break;
    };
  return (0);
}
