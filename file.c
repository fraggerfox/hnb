#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "node.h"
#include "tree.h"

static Node *npos;
static int startlevel;

static void
import_node (int level, int flags, char *data)
{

  level = level + startlevel;

  while (nodes_left (npos) > level)
    npos = node_left (npos);

  if (nodes_left (npos) == level)
    npos = node_insert_down (npos);

  if (nodes_left (npos) < level)
    npos = node_insert_right (npos);

  node_setflags (npos, flags);
  node_setdata (npos, data);
}

static void
import_byte (byte)
{

#define xlevel1 0
#define xlevel2 1
#define xflags1 2
#define xflags2 3
#define xdata   4

  static int expect = xlevel1;

  static int level;
  static int flags;
  static char *data;
  static int pos;

  switch (expect)
    {
    case xlevel1:
      level = byte;
      expect = xlevel2;
      break;
    case xlevel2:
      level = level + byte * 256;
      expect = xflags1;
      break;
    case xflags1:
      flags = byte;
      expect = xflags2;
      break;
    case xflags2:
      flags = flags + byte * 256;
      expect = xdata;
      data = (char *) malloc (32);
      pos = 0;
      data[pos] = 0;
      break;
    case xdata:
      if (byte)
	{
	  data[pos] = byte;
	  data[pos + 1] = 0;
	  pos++;
	  if ((pos & 31) == 31)
	    {			/* increase the buffer every 32 chars */
	      data = (char *) realloc (data, pos + 32);
	    };
	}
      else
	{
	  import_node (level, flags, data);
	  free (data);
	  expect = xlevel1;
	};
      break;
    };
}

#define bufsize 4096

Node *
import_db (Node * node, char *filename)
{
  char *buf;
  int file;
  int pos, end;
  int eof = 0;

  npos = node;
  startlevel = nodes_left (node);

  buf = (char *) malloc (bufsize);
  file = open (filename, O_RDONLY);

  while (!eof)
    {
      end = read (file, &buf[0], bufsize);
      if (end != bufsize)
	eof = 1;
      pos = 0;

      while (pos < end)
	{
	  import_byte (buf[pos++]);
	};
    };
  free (buf);
  close (file);
  if (node_getflags (node) & F_temp)
    node = node_remove (node);
  return (node);
}

void
export_node (int file, int level, int flags, char *data)
{
  char temp;

  temp = level & 255;
  write (file, &temp, 1);
  temp = (level / 256) & 255;
  write (file, &temp, 1);
  temp = flags & 255;
  write (file, &temp, 1);
  temp = (flags / 256) & 255;
  write (file, &temp, 1);
  write (file, data, strlen (data) + 1);
}

void
export_db (Node * node, char *filename)
{
  Node *tnode;
  int level, flags, startlevel;
  int file;
  char *data;

  file = creat (filename, 0660);
  startlevel = nodes_left (node);

  tnode = node;

  while ((tnode != 0) & (nodes_left (tnode) >= startlevel))
    {
      level = nodes_left (tnode) - startlevel;
      flags = node_getflags (tnode);
      data = node_getdata (tnode);
      export_node (file, level, flags, data);

      tnode = node_recurse (tnode);
    };

  close (file);
}

static void
ascii_export_node (FILE * file, int level, int flags, char *data)
{

  int cnt;

  for (cnt = 0; cnt < level; cnt++)
    {
      fprintf (file, "\t");
    };
/*FIXME ascii_export_node, doesn't export flags
  and reading has limitations on size/content*/
  fprintf (file, "%s\n", data);

}

Node *
ascii_import (Node * node, char *filename)
{
  int level, flags;
  char data[512];
  FILE *file;

  file = fopen (filename, "r");
  if (file == NULL)
    return (node);

  npos = node;
  startlevel = nodes_left (node);

  while (fgets (data, 512, file) != NULL)
    {
      flags = level = 0;
      data[strlen (data) - 1] = 0;
      while (data[level] == '\t')
	level++;
      import_node (level, flags, &data[level]);
    };

  fclose (file);

  if (node_getflags (node) & F_temp)
    node = node_remove (node);
  return (node);
}

void
ascii_export (Node * node, char *filename)
{
  Node *tnode;
  int level, flags, startlevel;
  char *data;
  FILE *file;

  file = fopen (filename, "w");
  startlevel = nodes_left (node);

  tnode = node;

  while ((tnode != 0) & (nodes_left (tnode) >= startlevel))
    {
      level = nodes_left (tnode) - startlevel;
      flags = node_getflags (tnode);
      data = node_getdata (tnode);
      ascii_export_node (file, level, flags, data);

      tnode = node_recurse (tnode);
    };

  fclose (file);
}
