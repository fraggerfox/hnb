#ifndef PREFS_H
#define PREFS_H

#define FORMAT_XML		0
#define FORMAT_ASCII	1
#define FORMAT_GXML		2
#define FORMAT_HTML		3

/* global struct used to remeber user preferences
*/
#ifndef PREFS_C
extern struct{
	int help_level;		/* which help menu to show */
	int view_debug;		/* display debug information */
	int collapse_mode;	/* which collapse mode is in action */
	int tutorial;		/* we we're loaded with the tutorial */
	int eleet_mode;		/* do eLi7e transform on displayed chars */
	int format;			/* what format are we currently editing */
	int def_format;		/* what is the format of the default database */
	char rc_file[100];	/* rc file loaded */
	char db_file[100];	/* path of database loaded */
	char default_db_file[100];	/* path to default database */
}prefs;
#endif
/*	initializes preferences with default values
*/
void init_prefs();

/*  load preferences, from prefs.rc_file
*/
void load_prefs();

/*  saves preferences, to prefs.rc_file
*/
void save_prefs();

#endif /* PREFS_H */
