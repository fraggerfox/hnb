#include <stdio.h>
#include <assert.h>
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
file, (if no file is specified, it loads %s)\n\
\n\
hierarchical notebook is distributed under the GNU General Public License\n\
\n\
Options include:\n\
\n\
        -h  or -help        this message\n\
        -v  or -version     prints the version\n\
\n\n", db_file);
}

#define BUFFERLENGTH 2048
#warning BUFFERLENGTH is currently set to 2048,.. should be removed and replaced by rigid code

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
  c = UI_RIGHT;

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
	  if (cursor_pos)
	    {
	      memmove (&input[cursor_pos], &input[cursor_pos + 1],
		       strlen (input) - cursor_pos);
	      input[cursor_pos - 1] = '_';
	      cursor_pos--;
	    };
	  break;
	  /*ignored keypresses..*/
	case UI_MENU:
	case UI_INSERT:
	  break;	  
	default:	  
	  memmove (&input[cursor_pos + 1], &input[cursor_pos],
		   strlen (input) - cursor_pos+1);
	  input[cursor_pos] = c;

	  cursor_pos++;
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
	    if(!(node_getflags(pos)&F_temp)){
	    pos = node_insert_down (pos);
		};
	    node_swap (pos, marked);
	    node_remove (marked);
	    stop = 1;
	    break;
	  default:
	    stop=1;
	    break;
	  };
      };
    stop = 0;
  };
}

char db_file[100];

int
app_quit ()
{
  int c;
  ui_draw (pos, input, UI_MODE_QUIT);
  c = ui_input ();
  switch (c)
    {
    case 'y':
    case 'Y':
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
    default:
      return (0);
    };
}


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
				node_swap(pos,node_down(pos));
				changes++;
			};
			pos=node_down(pos);
		};
		if(!changes)return(node);
	};

	return(node);
}

int
app_menu ()
{
  int c;
  ui_draw (pos, input, UI_MODE_MENU);
  c = ui_input ();
  switch (c)
    {
    case 's':
    case 'S':
    case 'd':
    case 'D':	
      ascii_export ( tree_root(), db_file);	
      break;
	case 'q':
	case 'Q':
      return (1);
      break;
	case 'x':
	case 'X':
      ascii_export ( tree_root(), db_file);		
      return (1);
      break;
	case 'o':
	case 'O':
		pos=node_sort_siblings(pos);
		return(0);
		break;
	case 't':		
	  ui_draw(pos, node2path(pos), UI_MODE_CONFIRM);
	  c=ui_input();
    default:
      return (0);
    };
	
	return 0;
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
  int stop = 0;
  while (!stop)
    {
      int c;
      ui_draw (pos, input, UI_MODE_NAVIGATE);
      c = ui_input ();
      switch (c)
	{
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
		  node_setflags (pos, 0);
		  pos = node_insert_right (pos);
		  node_setflags (pos, F_temp);
		};
	    };
	  input[0] = 0;
	  break;
	case UI_REMOVE:
	    app_remove();
	  break;
	case '\t':
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
	    stop = app_menu ();
	  };
	  input[0] = 0;
	  break;
	case UI_ENTER:
	  if (!strlen (input)) {
	    app_edit ();
	  } else {
	      if (node_getflags (pos) & F_temp)
		{
		  node_setflags (pos, 0);
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
	    app_edit();
	  break;
	case UI_MARK:
	  app_mark ();
	  break;
	case UI_MENU:
	  stop=app_menu ();
	  break;
	default:
	  input[strlen (input) + 1] = 0;
	  input[strlen (input)] = c;
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
      if (!strcmp (sw, "-h") || !strcmp (argv[i], "-help"))
	{
	  usage (argv[0]);
	  exit (0);
	}
	  else if (!strcmp (sw, "-v") || !strcmp (argv[i], "-version"))
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
