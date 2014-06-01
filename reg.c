#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "registry.h"

int main (int argc, char **argv)
{
  char db_file[100];

#ifndef WIN32
      sprintf (db_file, "%s/.reg", getenv ("HOME"));
#endif
#ifdef WIN32
      sprintf (db_file, "C:\\test.reg");
#endif

  if(argc!=3){
  	printf("usage: reg <set/get> /data/path\n");
	return(0);
  };
  
  if(!strcmp(argv[1],"set")){
    reg_load(db_file);
  	reg_set(argv[2]);
	reg_save(db_file);
	reg_close();
  };

  if(!strcmp(argv[1],"get")){
    reg_load(db_file);
	printf("%s\n",reg_get(argv[2]));
	reg_close();	
  };

  return(0);
}
