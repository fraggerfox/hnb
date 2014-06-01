#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "ui.h"
#include "prefs.h"
#include "ui_edit.h"
#include "ui_prefs.h"

#define BUFFERLENGTH 4096

void ui_prefs (Node *document_pos)
{
	Node *pos;
	char input[BUFFERLENGTH] = "";
	int stop = 0;
	int percent = prefs.showpercent;
	int collapse = prefs.collapse_mode;

	pos = load_prefs ();
	prefs.showpercent = 0;

	while (!stop) {
		int action;

		ui_draw (pos, pos, input, ui_mode_prefs);
		action = parsekey (ui_input (), 0);
		switch (action) {
			case k_prefs_cancel:
				stop = 1;
				break;
			case k_prefs_save:
				save_prefs (pos);
				ui_draw (document_pos, document_pos, input, ui_mode_help0 + prefs.help_level);
				infof (" saved config in %s", prefs.rc_file);

				tree_free (pos);
				prefs.showpercent = percent;
				prefs.collapse_mode = collapse;
				return;
				break;
			case k_prefs_apply:
				apply_prefs (pos);
				percent = prefs.showpercent;
				prefs.showpercent = 0;
				break;
			case k_prefs_siblingup:
				if (node_up (pos))
					pos = node_up (pos);
				break;
			case k_prefs_siblingdown:
				if (node_down (pos))
					pos = node_down (pos);
				break;
			case k_prefs_parent:
				if (node_left (pos))
					pos = node_left (pos);
				break;
			case k_prefs_siblingtop:
				pos = node_top (pos);
				break;
			case k_prefs_siblingbottom:
				pos = node_bottom (pos);
				break;
			case k_prefs_child:
				if (node_right (pos) && node_getpriority (pos) != 4) {
					pos = node_right (pos);
				}
				break;
			case k_prefs_remove:
				switch node_getpriority
					(pos) {
					case 4:	/*shortcut */
					{
						char data[255];

						node_setdata (node_right (pos), "-1");
						node_setdata (node_down (node_right (pos)), "-1");
						node_setdata (node_down
									  (node_down (node_right (pos))), "-1");
						node_setdata (node_down
									  (node_down
									   (node_down (node_right (pos)))), "-1");
						strcpy (data, node_getdata (pos));
						if (strrchr (data, '.')) {
							sprintf (strrchr (data, '.') + 1, " ");
							node_setdata (pos, data);
						}
						ui_draw (pos, pos, input, ui_mode_prefs);
						infof (" custom keybinding removed %s", "");
					}
						break;
					}
				break;
			case k_prefs_edit:
				switch node_getpriority
					(pos) {
					case 1:
						node_toggleflag (pos, F_done);	/* checkbox */
						break;
					case 2:{	/* radio button */
						Node *tnode = node_top (pos);

						while (tnode) {
							if (node_getpriority (tnode) == 2)
								node_setflag (tnode, F_done, 0);
							tnode = node_down (tnode);
						}
						node_setflag (pos, F_done, 1);
						break;
					}
					case 3:	/* editable item */
						ui_edit (pos, 1 /*restricted edit */ );
						break;
					case 4:	/* shortcut */
					{
						int t;
						char data[255];
						int values[4];

						values[0] = atoi (node_getdata (node_right (pos)));
						values[1] =
							atoi (node_getdata
								  (node_down (node_right (pos))));
						values[2] =
							atoi (node_getdata
								  (node_down (node_down (node_right (pos)))));
						values[3] =
							atoi (node_getdata
								  (node_down
								   (node_down
									(node_down (node_right (pos))))));

						infof
							(" press the keycombo that should trigger the action%s",
							 "");
						t = getch ();

						/* check for duplicate value */

						if (t == values[0] || t == values[1] || t == values[2]
							|| t == values[3]) {
							ui_draw (pos, pos, input, ui_mode_prefs);
							ui_draw (pos, pos, input, ui_mode_prefs);
							infof (" %s already assigned to action",
								   tidy_keyname (keyname (t)));
						} else {
							if (values[0] == -1) {
								values[0] = t;
							} else if (values[1] == -1) {
								values[1] = t;
							} else if (values[2] == -1) {
								values[2] = t;
							} else if (values[3] == -1) {
								values[3] = t;
							} else {
								ui_draw (pos, pos, input, ui_mode_prefs);
								ui_draw (pos, pos, input, ui_mode_prefs);
								infof
									(" Maximum four bindings can be on one action, just must delete first%s",
									 "");
								break;
							}

							sprintf (data, "%i", values[0]);
							node_setdata (node_right (pos), data);
							sprintf (data, "%i", values[1]);
							node_setdata (node_down (node_right (pos)), data);
							sprintf (data, "%i", values[2]);
							node_setdata (node_down
										  (node_down (node_right (pos))),
										  data);
							sprintf (data, "%i", values[3]);
							node_setdata (node_down
										  (node_down
										   (node_down (node_right (pos)))),
										  data);



							strcpy (data, node_getdata (pos));
							if (strrchr (data, '.')) {
								char name[4][20];

								strcpy (name[0],
										values[0] !=
										-1 ?
										tidy_keyname (keyname (values[0])) :
										"");
								strcpy (name[1],
										values[1] !=
										-1 ?
										tidy_keyname (keyname (values[1])) :
										"");
								strcpy (name[2],
										values[2] !=
										-1 ?
										tidy_keyname (keyname (values[2])) :
										"");
								strcpy (name[3],
										values[3] !=
										-1 ?
										tidy_keyname (keyname (values[3])) :
										"");
								sprintf (strrchr (data, '.') + 1,
										 " %s %s %s %s", name[0], name[1],
										 name[2], name[3]);
								node_setdata (pos, data);
							}
							ui_draw (pos, pos, input, ui_mode_prefs);
							ui_draw (pos, pos, input, ui_mode_prefs);
							infof (" %s assigned to action", keyname (t));
						}
					}
						break;
					default:
						if (node_getpriority (node_right (pos)) == 3) {
							pos = node_right (pos);
							ui_edit (pos, 1);
						}
						break;
					}
				break;
		}
	}
	tree_free (pos);
	prefs.showpercent = percent;
	prefs.collapse_mode = collapse;
}
