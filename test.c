#include "curses.h"

int
main ()
{
  int c=0;

  initscr();
  keypad (stdscr, TRUE);
  nonl();
  cbreak ();

  while(c!='x'){
	  c = getch ();
	  addstr("pressed [");
	  addstr(keyname(c));
	  printf("(%i)",c);
	  addstr("]\n");
  };
  
  endwin();

  return (0);
}
