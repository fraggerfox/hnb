#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"
#include "tree.h"
#include "tokenizer.h"

/*
 converts a pathstring to a node in the
 tree, ps, the tokenizer escapes
 double //'s as a single / without
 splitting the things at the sides
 a /// means,.. a slash at the end
 of token, and new token coming
*/

Node *
path2node (char *path)
{
  char *token;
  Node *node; /* should perhaps be a relative start node?*/

  node=tree_root();

  token_seperator='/';	

  token=tokenize(path);
  if(!strcmp(token,"/")){
  	node = tree_root();
    token=tokenize("");
	if(token[0]!=0)
    	node=node_exact_match(token,node);
  } else{
    node=node_exact_match(token,node);
  };
    token=tokenize("");

  while(token[0]!=0)
    { if(node_right(node))
    	node=node_right(node);
      if(strcmp(token,"/")) /* it is not the lonely / at the end*/
      	 node=node_exact_match(token,node);
      token=tokenize("");
    };

  return (node);
}


/*
 same as above,.. but forcing and recursive
 (creates the nodes as we go if they don't
 exist)
*/

Node *
path2node_make (char *path)
{
  char *token;
  Node *node; /* should perhaps be a relative start node?*/

  node=tree_root();

  token_seperator='/';	

  token=tokenize(path);
  if(!strcmp(token,"/")){
  	node = tree_root();
    token=tokenize("");
	if(token[0]!=0){
		Node *tnode;
		tnode=node_exact_match(token,node); /* something is amiss here? */
		if(tnode==0){
			tnode=node_insert_down(node);
			node_setdata(tnode,token);
		};
		node=tnode;
		};
  } else{
    node=node_exact_match(token,node);
  };
    token=tokenize("");

  while(token[0]!=0)
    { if(node_right(node)){
    	node=node_right(node);
        if(strcmp(token,"/")){ /* it is not the lonely / at the end*/
       	   Node *tnode;
		   tnode=node_exact_match(token,node);
		   if(tnode==0){
			 tnode=node_insert_down(node);
			 node_setdata(tnode,token);
		   };		 
		node=tnode;
		}
	  } else { /* we must create a child */
    	node=node_insert_right(node);
        if(strcmp(token,"/")){ /* it is not the lonely / at the end*/
			 node_setdata(node,token);
		   };
	  };		 
      token=tokenize("");
    };


/* if there is a node below,.. we should remove it,... 
   we don't want duplicate entries..
*/

  if(node_up(node))
  	node=node_remove(node_up(node));

  return (node);
}

/*
	creates an absolute path like 
	/aaa/bbb/ccc
	for the node (ccc) specified
	
	FIXME: it uses a static char array,.. that is reused..
	probably not the best way.. but..
*/

char *
node2path (Node * node)
{
  static char path[512];
  int pos=0;
  int levels = nodes_left (node);
  int cnt;

  path[pos]='/';path[++pos]=0;

  for (cnt = levels; cnt >= 0 ; cnt--){
      int cnt2;
	  Node *tnode=node;

      for (cnt2 = 0; cnt2 < cnt; cnt2++)
	      tnode = node_left(tnode);

	  strcpy(&path[pos], tnode->data);
	  pos=strlen(path);
	  path[pos]='/';path[++pos]=0;
    };
	
	path[--pos]=0;

  return(path);
}


char *
node2no_path (Node * node)
{
  static char path[512];
  int pos=0;
  int levels = nodes_left (node);
  int cnt;

  path[pos]='/';path[++pos]=0;

  for (cnt = levels; cnt >= 0 ; cnt--){
      int cnt2;
	  Node *tnode=node;

      for (cnt2 = 0; cnt2 < cnt; cnt2++)
	      tnode = node_left(tnode);

	  /*strcpy(&path[pos], tnode->data);*/
	  sprintf(&path[pos], "%i", nodes_up(tnode));
	  pos=strlen(path);
	  path[pos]='/';path[++pos]=0;
    };
	
	path[--pos]=0;

  return(path);
}

/*
	/0/3/2/
	means:
		/ goto root
		0 go down none
		/ go right
		3 go down three
		/ go right
		2 go down two
		/ go right
*/

Node *
no_path2node (char *path)
{
  char *token;
  Node *node; /* should perhaps be a relative start node?*/

  node=tree_root();

  token_seperator='/';	

  token=tokenize(path);
  if(!strcmp(token,"/")){
  	node = tree_root();
    token=tokenize("");
	if(token[0]!=0){
		int no=atoi(token);
		for(;no>0;no--)
			node=node_down(node);
	};

  } else{
	int no=atoi(token);
	for(;no>0;no--)
		node=node_down(node);
  };
    token=tokenize("");

  while(token[0]!=0)
    { if(node_right(node))
    	node=node_right(node);
      if(strcmp(token,"/")) /* it is not the lonely / at the end*/
	{
		int no=atoi(token);
		for(;no>0;no--)
			node=node_down(node);
	};	 
      token=tokenize("");
    };

  return (node);
}
