/*	loads the specified file 'registry' into memory
*/	void reg_load(char *registry);


/*	saves the contents of registry memory in to file 'registry'
*/	void reg_save(char *registry);


/*	frees the memory used by the registry
*/	void reg_close();


/*	get a value from the registry,
	e.g. reg_get("/prefs/res/") will return a string like "640x480" if there is
	a subnode 640x480, under res
	
	it will return "" if no match was found
	
    the pointer returned is a pointer to the registry internal string,.. and needn't be
	freed until closing the registry
*/	char *reg_get(char *data);


/*	sets a value in the registry,.. e.g.
	reg_set("/prefs/res/800x600") will create all the "directories" needed,
	and replace the rightmost value if one is preexistant
*/	void reg_set(char *data);


int reg_get_int (char *path);
void reg_set_int (char *path, int data);
