/*
 * tree_sort.c -- function to sort a level of nodes
 *
 * Copyright (C) 2001,2003 Øyvind Kolås <pippin@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "cli.h"


#if 0
static int cmp_ascdata (Node *a, Node *b)
{
	return (strcmp (a->data, b->data));
}

static int cmp_descdata (Node *a, Node *b)
{
	return (strcmp (b->data, a->data));
}
#endif

static int cmp_todo (Node *a, Node *b)
{
	if (!(a->flags) && !(b->flags))	/* accessing data directly for speed, */
		return (strcmp (a->data, b->data));

	if ((a->flags & F_todo) < (b->flags & F_todo))
		return 1;				/*  all todos at top */
	if ((a->flags & F_todo) > (b->flags & F_todo))
		return -1;
	if ((a->flags & F_todo) == (b->flags & F_todo)) {
		if (a->flags & F_todo) {
			if ((a->flags & F_done) > (b->flags & F_done))
				return 1;		/* not done */
			if ((a->flags & F_done) < (b->flags & F_done))
				return -1;

/*
			if ((a->priority ? a->priority : 3) >
				(b->priority ? b->priority : 3))
				return 1;		
			if ((a->priority ? a->priority : 3) <
				(b->priority ? b->priority : 3))
				return -1;
*/
		}
	}
	return (strcmp (a->data, b->data));
}

/* this quicksort costs less cputime, but costed a hell of a lot more braintime
   than the bubble sort */

/* apparantly mergesort is more effective for a doublelinked list 
 * (which as set of siblings actually is),. I might change it
 * when I get the time,.. but until then it works quite flawlessly */

static Node *quicksort (Node *Top, Node *Bottom,
						int (*cmp) (Node *a, Node *b))
{
	Node *part, *j;

	if (Bottom && Top && Bottom != Top
		&& (Bottom != Top && node_down (Bottom) != Top)) {
		j = node_up (Bottom);

		if (j == Top) {
			if (cmp (Top, Bottom) >= 0)
				node_swap (Top, Bottom);
		} else {
			part = Top;
			while (part != j) {
				while (cmp (part, Bottom) < 0)
					part = node_down (part);
				while (j && j != part && cmp (j, Bottom) >= 0)
					j = node_up (j);
				if (!j || node_up (part) == j)
					break;
				node_swap (part, j);
			}
			if (part != Bottom)
				node_swap (part, Bottom);

			quicksort (Top, node_up (part), cmp);	/* Sort  upper part */
			if (node_down (part))
				quicksort (node_down (part), Bottom, cmp);	/* sort lower part */
		}
	}
	return (Top);
}

Node *node_sort_siblings (Node *node)
{
	return (quicksort (node_top (node), node_bottom (node), cmp_todo));
}

/*	returns an number between 0 and 1000 showing the completion of this node,
	computed as follows:
		all children with todo boxes count as one part
		if a child is done count+= 1000/parts
		if a child is not done count+= node_calc_complete(child)/parts	
*/

/*
	TODO: should add criteries for ascending/descending,.. 
	usage of done status etc..
*/
static int sort_cmd (char *params, void *data)
{
	Node *pos = (Node *) data;

	return (int) node_sort_siblings (pos);
}

/*
!init_sort();
*/
void init_sort(){
		cli_add_command ("sort", sort_cmd, "");
		cli_add_help("sort","Sorts the siblings of the currently selected node");
}
