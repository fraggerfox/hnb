#include <curses.h>

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

#define UI_MARK 0

#define UI_IGNORE 1111

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

void ui_init();
void ui_draw(Node *node,char *input, int mode);
void ui_end();
int ui_input();
