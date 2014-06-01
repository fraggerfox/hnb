#ifndef PREFS_H
#define PREFS_H

#define FORMAT_HNB		0
#define FORMAT_ASCII	1
#define FORMAT_XML		2
#define FORMAT_HTML		3
#define FORMAT_LIBXML	4

#define RC_REV 2

/* global struct used to remeber user preferences
*/

typedef struct{
	int def_help_level;
	int help_level;
	int forced_up;
	int forced_down;
	int usertag;
	char usertags[28][40];
	int def_collapse_mode;
	int collapse_mode;
	int def_format;
	int format;
	int xml_cuddle;
	int xml_highlight;
	int tutorial;
	int eleet_mode;
	int debug;
	char rc_file[100];
	char db_file[100];
	char default_db_file[100];
	int fg_menuitm;
	int bg_menuitm;
	int bold_menuitm;
	int fg_menutxt;
	int bg_menutxt;
	int bold_menutxt;
	int bg;
	int fg_node;
	int bg_node;
	int bold_node;
	int fg_nodec;
	int bg_nodec;
	int bold_nodec;
	int fg_bullet;
	int bg_bullet;
	int fg_priority;
	int bg_priority;
	int bold_priority;
	int bold_bullet;
	int rc_rev;  /* to keep track of rc revisions */
	int mouse;	/* allow mouse navigation */
	int indent;
	int bulletmode;
	int showpercent;
}Tprefs;

extern Tprefs prefs;

/*	initializes preferences with default values
*/
void init_prefs();

/*  load preferences, from prefs.rc_file
*/
Node *load_prefs();

/*  saves preferences, to prefs.rc_file
*/
void save_prefs(Node *node);

void apply_prefs(Node *node);

void write_def_rc();
#endif /* PREFS_H */
