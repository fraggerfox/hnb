#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"
#include "tree.h"
#include "ui.h"
#include "file.h"
#include "version.h"
#include "path.h"

static void
usage (const char *av0)
{
  char db_file[100];

#ifndef WIN32
  sprintf (db_file, "%s/.hnb", getenv ("HOME"));
#endif
#ifdef WIN32
  sprintf (db_file, "C:\\hnb_data");
#endif
  fprintf (stderr, "\nusage: %s [options] [file] \n", av0);
  fprintf (stderr, "\n\
Hierarchical NoteBook by Øyvind Kolås <pippin@users.sourceforge.net>\n\
It is distributed under the GNU General Public License\n\
\n\
if no file is specified, the file '%s' is loaded/created\n\
\n\
Options include:\n\
\n\
\t-h  or --help\t\tthis message\n\
\t-v  or --version\tprints the version\n\
\n\n", db_file);
}

#define BUFFERLENGTH 4096
#warning BUFFERLENGTH is currently set to 4096,.. should be removed and replaced by rigid code

char input[BUFFERLENGTH];
Node *pos;

void
app_edit ()
{
  int c;
  int stop = 0;
  static int cursor_pos;
  static char *data_backup;
  static char input[BUFFERLENGTH];

  data_backup = pos->data;
  input[0] = 0;
  strcpy (&input[0], data_backup);
  pos->data = &input[0];
  cursor_pos = strlen (input);
  input[cursor_pos] = '_';
  input[cursor_pos + 1] = 0;
  input[cursor_pos + 2] = 0;
  c = 0;

  while (!stop)
    {
      ui_draw (pos, "", UI_MODE_EDIT);
      c = ui_input ();
      switch (c)
	{
	case UI_RIGHT:
	  if (cursor_pos < (strlen (input) - 1))
	    {
	      char tmp;
	      tmp = input[cursor_pos + 1];
	      input[cursor_pos + 1] = '_';
	      input[cursor_pos++] = tmp;
	    };
	  break;
	case UI_LEFT:
	  if (cursor_pos)
	    {
	      char tmp;
	      tmp = input[cursor_pos - 1];
	      input[cursor_pos - 1] = '_';
	      input[cursor_pos--] = tmp;
	    };
	  break;
	case UI_TOP:
	  while (cursor_pos)
	    {
	      char tmp;
	      tmp = input[cursor_pos - 1];
	      input[cursor_pos - 1] = '_';
	      input[cursor_pos--] = tmp;
	    };
	  break;
	case UI_BOTTOM:
	  while (cursor_pos < (strlen (input) - 1))
	    {
	      char tmp;
	      tmp = input[cursor_pos + 1];
	      input[cursor_pos + 1] = '_';
	      input[cursor_pos++] = tmp;
	    };
	  break;	  
	case UI_ESCAPE:
	  strcpy (&input[0], data_backup);
	  pos->data = &input[0];
	  cursor_pos = strlen (input);
	  input[cursor_pos] = '_';
	  input[cursor_pos + 1] = 0;
	  stop = 1;
	  break;	  
	case UI_UP:
	case UI_DOWN:
	case UI_PUP:
	case UI_PDN:	
	case UI_ENTER:
	  stop = 1;
	  break;
	case UI_BACKSPACE:
	case UI_BACKSPACE2:	
	case UI_BACKSPACE3:		
	  if (cursor_pos)
	    {
	      memmove (&input[cursor_pos], &input[cursor_pos + 1],
		       strlen (input) - cursor_pos);
	      input[cursor_pos - 1] = '_';
	      cursor_pos--;
	    };
	  break;
	  /*ignored keypresses..*/
	case UI_INSERT:
	  break;	  
    default:
	  if( c>31 && c<255){   /*  input for buffer */
	  memmove (&input[cursor_pos + 1], &input[cursor_pos],
		   strlen (input) - cursor_pos+1);
	  input[cursor_pos] = c;

	  cursor_pos++;
	  } else {              /* undefined keycode */
	    char msg[50];
  	    sprintf(msg," No action assigned to '%s'(%id) in edit-mode",keyname(c),c);
	    ui_draw(pos, msg, UI_MODE_ERROR);
		sleep(1);
	  }
	  break;	  
	  
	  
	};
    };

  /* position our cursor at the end */
  while (cursor_pos < (strlen (input) - 1))
    {
      char tmp;
      tmp = input[cursor_pos + 1];
      input[cursor_pos + 1] = '_';
      input[cursor_pos++] = tmp;
    };
  input[cursor_pos] = 0;

  pos->data = data_backup;
  node_setdata (pos, input);
  stop = 0;
}

void
app_mark ()  /* add visual cue in this one? */
{
  {
    int stop = 0;
    Node *marked = pos;
    input[0] = 0;
    while (!stop)
      {
	int c;
	ui_draw (pos, input, UI_MODE_MARKED);
	c = ui_input ();
	switch (c)
	  {
	  case UI_UP:
	    if (node_up (pos))
	      pos = node_up (pos);
	    break;
	  case UI_DOWN:
	    if (node_down (pos))
	      pos = node_down (pos);
	    break;
	  case UI_LEFT:
	    if (node_left (pos)){
		  if(node_getflags(pos)&F_temp){
		     pos=node_remove(pos);
		  } else {
	      pos = node_left (pos);
		  };
  	    };
	    break;
	  case UI_RIGHT:
	    if (pos != marked) {
	      if (node_right (pos)){
			pos = node_right (pos);
		  } else {
		  	if(!(node_getflags(pos)&F_temp)){
			  	pos = node_insert_right (pos);
				node_setflags(pos,F_temp);
			};
		  };
		};			
	    break;
	  case ' ':
	  case UI_ENTER:
	    if(!(node_getflags(pos)&F_temp)){
	    pos = node_insert_down (pos);
		};
	    node_swap (pos, marked);
	    node_remove (marked);
	    stop = 1;
	    break;
      case UI_ESCAPE:
	  case 'c':
	  case 'C':
	    stop = 1;
	  break;
	  default:
	{   /* undefined keycode */
	    char msg[50];
  	    sprintf(msg," No action assigned to '%s'(%id) in mark-mode",keyname(c),c);
	    ui_draw(pos, msg, UI_MODE_ERROR);
		sleep(1);
	  }
	  break;
	  };
      };
    stop = 0;
  };
}

char db_file[100];

int				/* queries user wether to quit or not, returns 1 when quitting*/
app_quit ()
{
  int c;
  ui_draw (pos, input, UI_MODE_QUIT);
  c = ui_input ();
  switch (c)
    {
    case 'y':
    case 'Y':
	case 'x':
	case 'X':
	case UI_QUIT:
      ascii_export ( tree_root(), db_file);
      return (1);
      break;
    case 'q':
    case 'Q':
      return (1);
	case 's':
	case 'S':
		ascii_export ( tree_root(), db_file);
		return(0);
		break;
	case 'C':case 'c':
	case UI_ESCAPE:
		return(0);
    default:
		{/* undefined keycode */
	    char msg[50];
  	    sprintf(msg," No action assigned to '%s'(%id) in quit-mode",keyname(c),c);
	    ui_draw(pos, msg, UI_MODE_ERROR);
		sleep(1);
	    }
	
      return (0);
    };
}
/* bubble sorts the siblings of the current node, returns the same node as the 
prior */

Node *node_sort_siblings(Node *node){
	Node *pos;
	int pass,passes;
	int changes;
	int item;
	pos=node_top(node);

	passes=nodes_down(pos);

	for(pass=0;pass<passes;pass++){
		pos=node_top(pos);	
		changes=item=0;
		while(item<passes-pass){
			item++;
			if(strcmp(node_getdata(pos),node_getdata(node_down(pos)))>0){
			    if(pos==node){
				  node=node_down(pos);
				} else {
				  if(node_down(pos)==node)
				     node=pos;
				}  
				node_swap(pos,node_down(pos));
				changes++;
			};
			pos=node_down(pos);
		};
		if(!changes)return(node);
	};
	return(node);
}

void app_search(){
  char *query[100];  
#warning somehow I must input this, but it kinda works now, yahoo!  
  Node *query_start=pos;
  int query_start_level=nodes_left(pos);
  strcpy((char *)query,"Find: ");
  ui_draw(pos,(char *)query,UI_MODE_GETSTR);  /* query user for search term */
  
  pos=node_recursive_match((char *)query,pos);
  
  while( pos!=0 && (nodes_left(pos)>=query_start_level))
    { int c;
      ui_draw( pos, (char *)query, UI_MODE_SEARCH);
      c=ui_input();
      switch(c){
        case 's':
        case 'S':
		case UI_ENTER:
          {
            return;
          }
          break;
        case 'c':
        case 'C':
		case UI_ESCAPE:
         {
          pos=query_start;
          return;
         }
         break;
        case 'n':
        case 'N':
		case ' ':		
		  pos=node_recursive_match((char *)query,pos);		
         break;
        default:
		{/* undefined keycode */
	    char msg[50];
  	    sprintf(msg," No action assigned to '%s'(%id) in search-mode",keyname(c),c);
	    ui_draw(pos, msg, UI_MODE_ERROR);
		sleep(1);
	    }
        break;
      };       
  };
  pos=query_start;
}

void app_remove(){
	  if(node_right(pos)){
	  	  int c;
	      ui_draw (pos, "node has children, really remove?", UI_MODE_CONFIRM);
		  c=ui_input();
		  if( (c=='y') || (c=='Y'))	  pos = node_remove (pos);
  
	  } else {
	  pos = node_remove (pos);
	  };
}

void
app_navigate ()
{
  int help_level=1;
  int stop = 0;
  while (!stop)
    {
      int c;
      ui_draw (pos, input, UI_MODE_HELP0+help_level);
      c = ui_input ();
      switch (c)
	{
	case UI_HELP:
	   help_level++;
	   if(help_level>=3)help_level=0;
	   break;
	case UI_QUIT:
	   stop=app_quit();
  	   break;
    case UI_FIND:
       app_search();      
      break;
	case UI_TOGGLE_TODO:
	  if(node_getflags(pos) & F_todo){
	     node_setflags(pos,node_getflags(pos)-F_todo);
	  }else{
	     node_setflags(pos,node_getflags(pos)+F_todo);
	  }
	  break;
	case UI_TOGGLE_DONE:
	  if(node_getflags(pos) & F_done){
	     node_setflags(pos,node_getflags(pos)-F_done);		  
		 if(node_left(pos) && (node_getflags(node_left(pos)) & F_done)){
		 	Node *tnode=node_left(pos);
		     node_setflags( tnode,node_getflags(tnode)-F_done);		  		 
		 }
	  }else{
	     node_setflags(pos,node_getflags(pos)+F_done);
		 if(node_left(pos) && !(node_getflags(node_left(pos)) & F_done)){
		 	Node *tnode=node_top(pos);
			int all_done=1;
			while(tnode!=0){
				if(!(node_getflags(tnode)&F_done))all_done=0;
				tnode=node_down(tnode);
			};
			if(all_done){
		 	Node *tnode=node_left(pos);
		       node_setflags( tnode,node_getflags(tnode)+F_done);
			}
		 }
	  }
	  break;
	case UI_SORT:
		pos=node_sort_siblings(pos);
	  break;
    case UI_LOWER:
        if(node_left(pos) ){
          Node *tnode;
          
          while(nodes_down(pos)){
            tnode=node_insert_down(node_left(pos));
            node_swap(tnode,node_bottom(pos));
            node_remove(node_bottom(pos));
          };
            tnode=node_insert_down(node_left(pos));
            node_swap(tnode,node_bottom(pos));
            node_remove(node_bottom(pos));
          pos=tnode;
        };
     break;  
    case UI_RAISE:
       if(node_up(pos)){    
         Node *tnode, *first_moved;
         pos=node_up(pos);                  /* go up  */             
             
         if(!(tnode=node_right(pos))){     /* must install a child */            
           tnode=node_insert_right(pos);
         } else {
           tnode=node_insert_down(node_bottom(tnode));
         }
         node_swap(node_down(pos),tnode);
         node_remove(node_down(pos));
         first_moved=tnode;

         while(node_down(pos)){
           tnode=node_insert_down(node_bottom(node_right(pos)));
           node_swap(node_down(pos),tnode);
           node_remove(node_down(pos));          
         };
         pos=first_moved;
       };
      break;
	case UI_TOP:
	  input[0] = 0;
	  pos = tree_root ();
	  break;
	case UI_UP:
	  if (node_up (pos))
	    pos = node_up (pos);
	  input[0] = 0;
	  break;
	case UI_DOWN:
	  if (node_down (pos))
	    pos = node_down (pos);
	  input[0] = 0;
	  break;
	case UI_PDN:
	  input[0] = 0;	
	  {int n;
	   for(n=0;n<10;n++) if (node_down (pos)) pos = node_down (pos);
	  };		  /* kan ikke få pg_up/pg_dn mengde fra ui,.. (ikke nå iallefall,..)
	  				 modul skillet er for godt definert*/
	  break;
	case UI_PUP:
	  input[0] = 0;
	  {int n;
	   for(n=0;n<10;n++) if (node_up (pos)) pos = node_up (pos);
	  };	
	  break;
	case UI_LEFT:
	  if (node_getflags (pos) & F_temp)
	    {
	      pos = node_remove (pos);
	    }
	  else
	    {
	      if (node_left (pos))
		pos = node_left (pos);
	    };
	  input[0] = 0;
	  break;
	case UI_RIGHT:
	  if (node_right (pos))
	    {
	      pos = node_right (pos);
	    }
	  else
	    {
	      if (input[0] | (!(node_getflags (pos) & F_temp)))
		{
		  pos = node_insert_right (pos);
		  node_setflags (pos, F_temp);
		  if(node_getflags( node_left(pos)) & F_todo)
		  	node_setflags(pos,F_temp+F_todo);
		};
	    };
	  input[0] = 0;
	  break;
	case UI_REMOVE:
	    app_remove();
	  break;
	case UI_COMPLETE:
	  if (strcmp (input, node_getdata (pos)) == 0){
	  		if (node_right (pos)) {
		      pos = node_right (pos);
		    } else {
		      if (input[0] | (!(node_getflags (pos) & F_temp))) {
				  node_setflags (pos, 0);
				  pos = node_insert_right (pos);
				  node_setflags (pos, F_temp);
			  };
	    	};
	  		input[0] = 0;	  
	  } else {
	      strcpy (input, node_getdata (pos));
	  };
	  break;
	case UI_ESCAPE:
	  if (node_getflags (pos) & F_temp) {
	      pos = node_remove (pos);
	  } else {
	      stop=app_quit();
		/* other purpose for escape?? quit perhaps? */
	  };
	  input[0] = 0;
	  break;
	case UI_ENTER:
	  if (!strlen (input)) {
	    app_edit ();
	  } else {
	      if (node_getflags (pos) & F_temp)
		{
		  node_setflags (pos, node_getflags (pos)- F_temp );
		}
	      else
		{
		  pos = node_insert_down (node_bottom (pos));
		  node_setdata (pos, input);
		};
	      input[0] = 0;
	    };
	  break;
	case UI_BACKSPACE:
	case UI_BACKSPACE2:	
	case UI_BACKSPACE3:
	  if (!strlen (input))
	    {
		  app_remove();
	    }
	  else
	    {
	      input[strlen (input) - 1] = 0;
	      if (node_getflags (pos) & F_temp)
		if (node_up (pos))
		  pos = node_remove (pos);
	    };
	  break;
	case UI_INSERT:
	    pos = node_insert_down(pos);
	      if(node_left(pos))
		  if(node_getflags( node_left(pos)) & F_todo)
		  	node_setflags(pos,F_todo);
			
		 if(node_left(pos) && (node_getflags(node_left(pos)) & F_done)){
		 	Node *tnode=node_left(pos);
		     node_setflags( tnode,node_getflags(tnode)-F_done);		  		 
		 }  /* unset parental node */
			
		
	    app_edit();
	  break;
	case UI_MARK:
	  app_mark ();
	  break;
	case UI_IGNORE:
	 break;
    default:
	  if( c>31 && c<255){   /*  input for buffer */
		  input[strlen (input) + 1] = 0;
		  input[strlen (input)] = c;
	  } else {              /* undefined keycode */
	    char msg[50];
  	    sprintf(msg," No action assigned to '%s'(%id) in navigation mode",keyname(c),c);
	    ui_draw(pos, msg, UI_MODE_ERROR);
		sleep(1);
	  }
	  break;
	};

      if (strlen (input))
	{
	  if (node_getflags (pos) & F_temp)
	    {
	      node_setdata (pos, input);
	    }
	  else
	    {
	      if (node_match (input, pos))
		{
		  if (strcmp
		      (node_getdata (node_match (input, pos)),
		       node_getdata (pos)) != 0)
		    pos = node_match (input, pos);
		}
	      else
		{
		  pos = node_insert_down (node_bottom (pos));
		  node_setflags (pos, F_temp);
		  node_setdata (pos, input);
	      if(node_left(pos))
			  if(node_getflags( node_left(pos)) & F_todo)
			  	node_setflags(pos,F_todo+F_temp);
		 if(node_left(pos) && (node_getflags(node_left(pos)) & F_done)){
		 	Node *tnode=node_left(pos);
		     node_setflags( tnode,node_getflags(tnode)-F_done);		  		 
		 }  /* unset parental node */				
		  
		  
		};
	    };
	};

    };
}

int
main (int argc, char **argv)
{
  int i;
  char *ui;
  db_file[0] = 0;

  for (i = 1; i < argc; i++)
    {
      const char *sw = argv[i];
      if (sw[0] == '-' && sw[1] == '-')
	sw++;
      if (!strcmp (sw, "-h") || !strcmp (argv[i], "--help"))
	{
	  usage (argv[0]);
	  exit (0);
	}
	  else if (!strcmp (sw, "-v") || !strcmp (argv[i], "--version"))
	{
	  printf("%s\n",VERSION);
	  exit (0);
	}
      else if (!strcmp (sw, "-ui"))
	{
	  ui = argv[++i];
	}
      else
	{
	  strcpy (&db_file[0], sw);
	}
    }

  if (db_file[0] == 0)
    {
#ifndef WIN32
      sprintf (db_file, "%s/.hnb", getenv ("HOME"));
#endif
#ifdef WIN32
      sprintf (db_file, "C:\\hnb_data");
#endif
    };

  input[0] = 0;
  tree_init ();

  pos = tree_root ();
  pos = ascii_import (pos, db_file);

  ui_init ();

  app_navigate ();

  ui_end ();
  return(0);
}
