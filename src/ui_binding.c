/*
 * ui_binding.c -- management of keybindings
 *
 * Copyright (C) 2001,2001 �yvind Kol�s <pippin@users.sourceforge.net>
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
#include "prefs.h"
#include <ctype.h>

#include "cli.h"				/* should be moved to keybinding file */
#include <string.h>				/* should be moved to keybinding file */
#include <stdlib.h>
#include <curses.h>

#include "ui.h"

static int scope_count = ui_scope_tempterm;

char *ui_scope_names[MAX_SCOPES] =
	{ "main", "confirm", "nodeedit", "lineedit" };

static char *ui_action_names[ui_action_terminator - 512] = {
	"backspace",
	"bkill_word",
	"bol",
	"bottom",
	"bskipword",
	"cancel",
	"command",
	"complete",
	"confirm",
	"delete",
	"down",
	"eol",
	"ignore",
	"join",
	"kill_line",
	"kill_to_bol",
	"kill_to_eol",
	"kill_word",
	"left",
	"pagedown",
	"pageup",
	"quit",
	"right",
	"skipword",
	"split",
	"top",
	"unbound",
	"up",
};


typedef struct {
	int code;
	char *name;
} keydbitm;

static keydbitm keydb[] = {
	{KEY_BREAK, "break"},
	{KEY_DOWN, "down"},
	{KEY_UP, "up"},
	{KEY_LEFT, "left"},
	{KEY_RIGHT, "right"},
	{KEY_HOME, "home"},
	{KEY_BACKSPACE, "backspace"},
	{KEY_F (1), "f1"},
	{KEY_F (2), "f2"},
	{KEY_F (3), "f3"},
	{KEY_F (4), "f4"},
	{KEY_F (5), "f5"},
	{KEY_F (6), "f6"},
	{KEY_F (7), "f7"},
	{KEY_F (8), "f8"},
	{KEY_F (9), "f9"},
	{KEY_F (10), "f10"},
	{KEY_F (11), "f11"},
	{KEY_F (12), "f12"},
	{KEY_DC, "del"},
	{KEY_IC, "ins"},
	{KEY_EIC, "eic"},
	{KEY_NPAGE, "npage"},
	{KEY_PPAGE, "ppage"},
	{KEY_ENTER, "enter"},
	{KEY_LL, "ll"},
	{KEY_A1, "a1"},
	{KEY_A3, "a3"},
	{KEY_B2, "b2"},
	{KEY_C1, "c1"},
	{KEY_C3, "c3"},
	{KEY_BEG, "beg"},
	{KEY_CANCEL, "cancel"},
	{KEY_CLOSE, "close"},
	{KEY_COMMAND, "command"},
	{KEY_COPY, "copy"},
	{KEY_CREATE, "create"},
	{KEY_END, "end"},
	{KEY_EXIT, "exit"},
	{KEY_FIND, "find"},
	{KEY_HELP, "help"},
	{KEY_MARK, "mark"},
	{KEY_MESSAGE, "message"},
	{KEY_MOVE, "move"},
	{KEY_NEXT, "next"},
	{KEY_OPEN, "open"},
	{KEY_OPTIONS, "options"},
	{KEY_PREVIOUS, "previous"},
	{KEY_REDO, "redo"},
	{KEY_REFERENCE, "reference"},
	{KEY_REFRESH, "refresh"},
	{KEY_REPLACE, "replace"},
	{KEY_RESTART, "restart"},
	{KEY_RESUME, "resume"},
	{KEY_SAVE, "save"},
	{KEY_SBEG, "sbeg"},
	{KEY_SCANCEL, "scancel"},
	{KEY_SCOMMAND, "scommand"},
	{KEY_SCOPY, "scopy"},
	{KEY_SCREATE, "screate"},
	{KEY_SDC, "sdc"},
	{KEY_SDL, "sdl"},
	{KEY_SELECT, "select"},
	{KEY_SEND, "send"},
	{KEY_SEOL, "seol"},
	{KEY_SEXIT, "sexit"},
	{KEY_SFIND, "sfind"},
	{KEY_SHELP, "shelp"},
	{KEY_SHOME, "shome"},
	{KEY_SIC, "sic"},
	{KEY_SLEFT, "sleft"},
	{KEY_SMESSAGE, "smessage"},
	{KEY_SMOVE, "smove"},
	{KEY_SNEXT, "snext"},
	{KEY_SOPTIONS, "soptions"},
	{KEY_SPREVIOUS, "sprevious"},
	{KEY_SPRINT, "sprint"},
	{KEY_SREDO, "sredo"},
	{KEY_SREPLACE, "sreplace"},
	{KEY_SRIGHT, "sright"},
	{KEY_SRSUME, "srsume"},
	{KEY_SSAVE, "ssave"},
	{KEY_SSUSPEND, "ssupsend"},
	{KEY_SUNDO, "sundo"},
	{KEY_SUSPEND, "suspend"},
	{KEY_UNDO, "undo"},
	{' ', "space"},
	{0, "^space"},
	{1, "^A"},
	{2, "^B"},
	{3, "^C"},
	{4, "^D"},
	{5, "^E"},
	{6, "^F"},
	{7, "^G"},
	{8, "backspace2"},
	{9, "tab"},
	{10, "^J"},
	{11, "^K"},
	{12, "^L"},
	{13, "return"},
	{14, "^N"},
	{15, "^O"},
	{16, "^P"},
	{17, "^Q"},
	{18, "^R"},
	{19, "^S"},
	{20, "^T"},
	{21, "^U"},
	{22, "^V"},
	{23, "^W"},
	{24, "^X"},
	{25, "^Y"},
	{26, "^Z"},
	{27, "esc"},
	{999, ".."},
	{1000, "any"},				/* special key used to trap all keys (i.e. avoid typing without editing) */
};


#define MAX_BINDINGS 255		/* maximum number of bound functions in one context */

Tbinding ui_binding[MAX_SCOPES][MAX_BINDINGS];
static int ui_binding_count[MAX_SCOPES] = { 0 };


static int string2scope (char *str)
{
	int j = 0;

	while (j < scope_count) {
		if (!strcmp (str, ui_scope_names[j]))
			return j;
		j++;
	}
	if (j < MAX_SCOPES - 2) {
		ui_scope_names[j] = strdup (str);
		scope_count++;
		return j;
	}
	printf ("scope limit exceeded\n");
	exit (0);
}


static int string2action (char *str)
{
	int j = 0;

	while (j < ui_action_terminator - 512) {
		if (!strcmp (ui_action_names[j], str))
			return j + 512;
		j++;
	}
	return -1;
}


static int string2keycode (char *str)
{
	int i;

	if (strlen (str) == 1)		/* a string consisting og just one char,.. is a single key */
		return str[0];

	for (i = 0; i < sizeof (keydb) / sizeof (keydbitm); i++)
		if (!strcmp (keydb[i].name, str))
			return (keydb[i].code);
	return -1;
}


int ui_current_scope = 0;

static uint64_t ui_context_cmd (int argc, char **argv, void *data)
{
	if(argc<2){
		cli_outfunf("usage: %s <contextname>",argv[0]);
		return PTR_TO_UINT64(data);
	}
	ui_current_scope = string2scope (argv[1]);
	return PTR_TO_UINT64(data);
}

static void makebinding (int scope_no, int key, int action, char *action_name,
						 char *action_params)
{
	ui_binding[scope_no][ui_binding_count[scope_no]].key = key;
	ui_binding[scope_no][ui_binding_count[scope_no]].action = action;
	ui_binding[scope_no][ui_binding_count[scope_no]].action_name =
		strdup (action_name);
	ui_binding[scope_no][ui_binding_count[scope_no]].action_param =
		strdup (action_params);
	ui_binding_count[scope_no]++;
}

static uint64_t ui_bind_cmd (int argc, char **argv, void *data)
{
/*	char context[40];*/
	char *key;
	char *action;

	if(argc<3){
		cli_outfunf("error in bindings %s %s %s %s\n",argv[0],argv[1],argv[2],argv[3]);
		return PTR_TO_UINT64(data);
	}
	key=argv[1];
	action=argv[2];

	if (string2action (action) != -1) {
		makebinding (ui_current_scope, string2keycode (key),
					 string2action (action), action, "");
	} else {
		makebinding (ui_current_scope, string2keycode (key),
					 ui_action_command, "command", action);
	}

	return PTR_TO_UINT64(data);
}



static Tbinding keyproxy = { 0, 0, "key", "key" };
Tbinding *lastbinding;

Tbinding *parsekey (int key, int scope)
{
	int j = 0;

	while (j < ui_binding_count[scope]) {
		if (key == ui_binding[scope][j].key) {
			lastbinding = &ui_binding[scope][j];
			return lastbinding;
		} else if (ui_binding[scope][j].key == 1000 /* anykey */ ) {
			lastbinding = &ui_binding[scope][j];
			*((int *) &lastbinding->action_param[0]) = key;
			return lastbinding;
		}

		j++;
	}
	keyproxy.action = key;
	keyproxy.key = key;
	lastbinding = &keyproxy;
	return lastbinding;
}

static char *unboundstr = "><";

char *resolve_binding (int scope, int action)
{
	int no = 0;

	while (no < ui_binding_count[scope]) {
		if (action == ui_binding[scope][no].action)
			return tidy_keyname (keyname (ui_binding[scope][no].key));
		no++;
	}
	return unboundstr;
}


/* 
 *	Function to make a keynames returned from curses a little nicer
 *
 */
char *tidy_keyname (const char *keyname)
{
	static char buf[100];

	strcpy (buf, keyname);
	if (!strncmp (buf, "KEY_", 4)) {
		memmove (buf, buf + 4, sizeof (buf) - 4);
	}

	if (buf[1] == '(') {
		memmove (buf + 1, buf + 2, sizeof (buf) - 3);	/* Function keys */
		if (buf[2] == ')')
			memmove (buf + 2, buf + 3, sizeof (buf) - 4);
		if (buf[3] == ')')
			memmove (buf + 3, buf + 4, sizeof (buf) - 5);
	}

	if (!strcmp (buf, "IC"))
		return "ins";
	if (!strcmp (buf, "DC"))
		return "del";
	if (!strcmp (buf, "UP"))
		return "up";
	if (!strcmp (buf, "^M"))
		return "return";
	if (!strcmp (buf, "^I"))
		return "tab";
	if (!strcmp (buf, "END"))
		return "end";
	if (!strcmp (buf, "^["))
		return "esc";
	if (!strcmp (buf, "^@"))
		return "^space";
	if (!strcmp (buf, " "))
		return "space";
	if (!strcmp (buf, "BACKSPACE"))
		return "backspace";
	if (strlen (buf) > 3) {
		char *c = buf;

		while (*c) {
			*c = tolower (*c);
			c++;
		}
	}
	return buf;
}


/*
!init_ui_binding();
*/

static uint64_t dummy_cmd (int argc, char **argv, void *data)
{
	return PTR_TO_UINT64(data);
}

void init_ui_binding ()
{
	cli_add_command ("bind", ui_bind_cmd, "<key> <action> [parameter]");
	cli_add_help ("bind",
				  "Binds the action, to the specified key, subsequent actions my be bound by using the virtual key '..' thus chaining them together into a small script.");
	cli_add_command ("context", ui_context_cmd, "<context>");
	cli_add_help ("context",
				  "Changes context, context may be one of 'main', 'lineedit', 'nodeedit', 'confirm' or any other userdefined context.");

/* dummy commands to show the 'builtins' */
/*	cli_add_command ("backspace", dummy_cmd, "");
	cli_add_help ("backspace", "erase the preceding character");
	cli_add_command ("delete", dummy_cmd, "");
	cli_add_help ("delete", "erase the following character");

	cli_add_command ("down", dummy_cmd, "");
	cli_add_command ("left", dummy_cmd, "");
	cli_add_command ("right", dummy_cmd, "");
	cli_add_command ("up", dummy_cmd, "");
	cli_add_help ("down", "moves the cursor or selection down");
	cli_add_help ("left", "moves the cursor or selection left");
	cli_add_help ("right", "moves the cursor or selection right");
	cli_add_help ("up", "moves the cursor or selection up");

	cli_add_command ("bkill_word", dummy_cmd, "");
	cli_add_command ("kill_word", dummy_cmd, "");
	cli_add_help ("bkill_word", "erases the preceding word");
	cli_add_help ("bkill_word", "erases the following word");

	cli_add_command ("bol", dummy_cmd, "");
	cli_add_help ("bol", "move cursor to beginning of line");

	cli_add_command ("top", dummy_cmd, "");
	cli_add_command ("bottom", dummy_cmd, "");
	cli_add_help ("top", "move to the topmost sibling of the current entry");
	cli_add_help ("bottom",
				  "move to the bottommost sibling of the current entry");

	cli_add_command ("bskipword", dummy_cmd, "");
	cli_add_command ("cancel", dummy_cmd, "");

	cli_add_command ("complete", dummy_cmd, "");
	cli_add_help ("complete",
				  "output possible completions of entered command or variable");

	cli_add_command ("confirm", dummy_cmd, "");
	cli_add_help ("confirm",
				  "confirm the question posed, (used in dialogs asking for the users' consent)");


	cli_add_command ("eol", dummy_cmd, "");
	cli_add_help ("eol", "move to the end of the line");

	cli_add_command ("ignore", dummy_cmd, "");

	cli_add_command ("join", dummy_cmd, "");
	cli_add_command ("split", dummy_cmd, "");

	cli_add_help ("join",
				  "join the currently edited node with the following sibling");
	cli_add_help ("split",
				  "split the currently edited node at the cursor location");

	cli_add_command ("kill_line", dummy_cmd, "");
	cli_add_help ("kill_line", "erase the currently edited line");

	cli_add_command ("kill_to_bol", dummy_cmd, "");
	cli_add_command ("kill_to_eol", dummy_cmd, "");

	cli_add_command ("pagedown", dummy_cmd, "");
	cli_add_command ("pageup", dummy_cmd, "");

	cli_add_command ("skipword", dummy_cmd, "");

	cli_add_command ("unbound", dummy_cmd, "");
	cli_add_help ("unbound",
				  "give a message indicating that the pressed key is not bound in the current context");
*/
}
