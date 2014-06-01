#ifndef PREFS_H
#define PREFS_H

enum {
	format_hnb = 0,
	format_ascii,
	format_xml,
	format_sxml,
	format_html,
	format_slides,
	format_help,
	format_terminator
};

extern const char *format_name[format_terminator];

#define RC_REV 8

/* global struct used to remeber user preferences
*/
typedef struct {
	int def_help_level;
	int help_level;
	int forced_up;
	int forced_down;
	int def_collapse_mode;
	int collapse_mode;
	int def_format;
	int format;
	int xml_cuddle;
	int xml_highlight;
	int tutorial;
	int debug;
	char rc_file[100];
	char db_file[100];
	char default_db_file[100];
	char query[100];
	int indent;
	int bulletmode;
	int showpercent;
	int keepwhitespace;
	int fixedfocus;
	int savepos;
} Tprefs;

extern Tprefs prefs;

/*	initializes preferences with default values
*/
void init_prefs ();

/*  load preferences, from prefs.rc_file
*/
void load_prefs (char *prefsfile);

void write_default_prefs ();

#endif /* PREFS_H */
