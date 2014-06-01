extern struct{
	int help_level;
	int view_debug;
	int collapse_mode;
	int tutorial;
	int eleet_mode;
	char rc_file[100];
	char db_file[100];
	char default_db_file[100];
}prefs;

void init_prefs();
void load_prefs();
void save_prefs();
