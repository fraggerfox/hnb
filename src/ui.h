#ifndef UI_H
#define UI_H
#include <curses.h>

/* actions checked for by hnb.c */

#define UI_RIGHT	KEY_RIGHT
#define UI_LEFT		KEY_LEFT
#define UI_DOWN		KEY_DOWN
#define UI_UP		KEY_UP

#define UI_PUP		KEY_PPAGE
#define UI_PDN		KEY_NPAGE

#define UI_REMOVE	KEY_DC
#define UI_ENTER	13
#define UI_INSERT	KEY_IC

#define UI_FIND 	6/* ctrl+f*/
#define UI_LOWER	2/* ctrl+b*/
#define UI_RAISE	14/* ctrl+n*/

#define UI_SPLIT	19/* ctrl+s*/
#define UI_JOIN		10/* ctrl+j*/
#define UI_PRIORITY	16/* ctrl+p*/

#define UI_PREFS	KEY_F(5)

#define UI_ESCAPE	27

/* these two wont work in my rxvts */
#define UI_TOP		KEY_HOME
#define UI_BOTTOM	KEY_END

/* must have both of these bound to backspace for some reason*/
/* the third one to work with win32 pdcurses thingy */
#define UI_BACKSPACE 127
#define UI_BACKSPACE2 KEY_BACKSPACE
#define UI_BACKSPACE3 8

#define UI_COMPLETE 9 /* tab */
#define UI_HELP		KEY_F(1)
#define UI_QUIT		24/* ctrl+x */
#define UI_SAVE		19 /* ctrl+s*/
#define UI_EXPORT   5/* ctrl+e */
#define UI_IMPORT   12/* ctrl+l */

#define UI_SORT		15/* ctrl+o*/

#define UI_TOGGLE_TODO 20/* ctrl+t */
#define UI_TOGGLE_DONE 4 /* ctrl+d */

#define UI_DEBUG	KEY_F(11)

#define UI_MARK 0  /* ctrl + space */

#ifdef WIN32

#undef UI_MARK
#define UI_MARK  7 /* ctrl+G*/

#endif

#define UI_IGNORE -1

/* different display modes of the curses interface */

#define UI_MODE_EDIT	 1
#define UI_MODE_MARKED	 2
#define UI_MODE_QUIT	 3
#define UI_MODE_CONFIRM  5
#define UI_MODE_ERROR    6
#define UI_MODE_INFO     7
#define UI_MODE_HELP0	 8
#define UI_MODE_HELP1	 9
#define UI_MODE_HELP2	 10
#define UI_MODE_GETSTR	 11
#define UI_MODE_SEARCH   12
#define UI_MODE_EXPORT   13
#define UI_MODE_IMPORT 	 14
#define UI_MODE_PREFS	 15
#define UI_MODE_EDITR	 16
#define UI_MODE_DEBUG 	 17


/* collapse modes */
#define COLLAPSE_ALL 0
#define COLLAPSE_ALL_BUT_CHILD 1
#define COLLAPSE_NONE 2
#define COLLAPSE_PATH 3
#define COLLAPSE_END 3
#define COLLAPSE_ONLY_SIBLINGS 4

/*color pairs used*/
#define UI_COLOR_MENUITM	1
#define UI_COLOR_MENUTXT	2
#define UI_COLOR_NODE		3
#define UI_COLOR_NODEC		4
#define UI_COLOR_BULLET		5
#define UI_COLOR_PRIORITY	6
#define UI_COLOR_BG			7

#define BULLET_NONE			0
#define BULLET_PLUSMINUS	1
#define BULLET_STAR			2
#define BULLET_MINUS		3

/* initializes the curses ui sets keyboard mode etc.
*/
void ui_init();

/* returns the terminal to the state it had before ui_init
*/
void ui_end();

/* draw the interface using curses, current node is node
   *input varies from mode to mode, in GETSTR mode input
   is owerwritten with the value input
*/
void ui_draw(Node *node,Node *lastnode,char *input, int mode);

/* waits until a curses event occurs, translates it according
   to the defines above in this file, and returns the new
   value. (also processes some of the events like resize itself)
*/
int ui_input();

/* used for approximately guessing how much we should skip 
   when page up/page down is requested.
*/
extern int hnb_edit_posup;
extern int hnb_edit_posdown;

#endif /* UI_H */

