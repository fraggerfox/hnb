/*
 * ui.c -- the part of hnb that calls curses
 *
 * Copyright (C) 2001-2003 �yvind Kol�s <pippin@users.sourceforge.net>
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "tree.h"
#include "curses.h"
#include "ui_overlay.h"
#include "prefs.h"
#include "cli.h"

#define UI_C
#include "ui.h"

int nodes_above;
int active_line;
int nodes_below;

void ui_init ()
{
	initscr ();
	clear ();
	intrflush (stdscr, TRUE);
	keypad (stdscr, TRUE);
	nonl ();
	raw ();						/* enable binding ctl+c,ctrl+q,ctrl+z .. */
	noecho ();
	active_line = LINES / 3;

	#ifdef NCURSES_VERSION
	/* 20021129 RVE - assign terminal default colors to color number -1 */
		use_default_colors();	
		
		define_key("\x7f",KEY_BACKSPACE);
		define_key("\033[11~",KEY_F(1));
		define_key("\033[28-",KEY_F(1));
		define_key("\033OP",KEY_F(1));
		define_key("\033[OP",KEY_F(1));
		
		define_key("\033[A",KEY_UP);
		define_key("\033OA",KEY_UP);
		define_key("\033[B",KEY_DOWN);
		define_key("\033OB",KEY_DOWN);
		define_key("\033[C",KEY_RIGHT);
		define_key("\033OC",KEY_RIGHT);
		define_key("\033[D",KEY_LEFT);
		define_key("\033OD",KEY_LEFT);
		
		
		define_key("\033[3~",KEY_DC);
		define_key("\033[7~",KEY_HOME);
		define_key("\033[8~",KEY_END);
		
		define_key("\033O2A",KEY_SPREVIOUS);
		define_key("\033[a",KEY_SPREVIOUS);
		define_key("\033O2B",KEY_SNEXT);
		define_key("\033[b",KEY_SNEXT);
		define_key("\033[c",KEY_SRIGHT);
		define_key("\033[d",KEY_SLEFT);
	#endif	


	cli_width=COLS;

	if (has_colors ()) {
		start_color ();
	}

	nodes_above = active_line;
	nodes_below = LINES - active_line;
}


void ui_end ()
{
	clear ();
	refresh ();
	endwin ();
}

/*extern Node *pos;
*/
int ui_input ()
{
	int c;

	c = getch ();
	switch (c) {
#ifdef KEY_RESIZE
		case KEY_RESIZE:
			active_line = LINES / 3;
			nodes_above = active_line;
			nodes_below = LINES - active_line;
			cli_width=COLS;
			c = getch ();
			return ui_action_ignore;
#endif
	}
	return (c);
}
