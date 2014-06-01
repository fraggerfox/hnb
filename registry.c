#include <string.h>
#include "node.h"
#include "tree.h"
#include "file.h"
#include "path.h"

char *reg_get(char *data){
	  Node *pos;
/* 	  char *retval;*/
	 
	  pos = path2node(data);

	  if(pos)
	  	return( pos->data);	  
	  return("");

/*	  naahh,.. we don't want to make a copy that must be freed,
		
	  if(pos)  
	    retval=(char *)strdup( pos->data );
	  else
	    retval=(char *)strdup("");
	  	  
	  return(retval);
*/	  
	  
}

void reg_set(char *data){
	  path2node_make(data);
}


void reg_load(char *registry){
	  tree_init();
	  ascii_import( tree_root(), registry);
}

void reg_save(char *registry){
	  ascii_export( tree_root(), registry);
}

void reg_close(){
	tree_free();
}

