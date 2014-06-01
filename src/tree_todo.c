/*
 * tree_todo.c -- functions relating to completion calculations
 *
 * Copyright (C) 2002,2003 �yvind Kol�s <pippin@users.sourceforge.net>
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
#include "prefs.h"

void node_update_parents_todo (Node *pos)
{
#if 0
	Node *tnode = node_top (pos);

	if (node_left (pos) && node_getflag (node_left (pos), F_todo)
		&& !node_getflag (pos, F_temp)) {

		int status = -1;

		while (tnode && status) {
			if (node_getflag (tnode, F_todo)) {
				status = 1;
				if (!node_getflag (tnode, F_done))
					status = 0;
			}
			tnode = node_down (tnode);
		}

		tnode = node_left (pos);

		if (status == 1) {		/* all todo nodes were checked */
			node_setflag (tnode, F_done, 1);
			node_update_parents_todo (tnode);
		} else if (status == 0) {	/* one or more todo node were unchecked */
			node_setflag (tnode, F_done, 0);
			node_update_parents_todo (tnode);
		}
	}
#endif
}

/* a traversal calculation of completion level
*/
int old_node_calc_complete (Node *node)
{
	int percent_sum = 0;
	int nodes = 0;

	if (!(node_getflag (node, F_todo)))
		return -1;				/* node has no completion status */

	if (node_getflag (node, F_done))
		return 2000;			/* this node is done */

	if (node_getpercent_done (node) != -1)
		return node_getpercent_done (node) * 10;

	if (!node_right (node))
		return 0;				/* no undone children,.. completly undone */

	node = node_right (node);

	while (node) {
		switch (old_node_calc_complete (node)) {
			case -1:
				break;
			case 2000:
				percent_sum += 1000;
				nodes++;
				break;
			default:
				percent_sum += old_node_calc_complete (node);
				nodes++;
				break;
		}
		node = node_down (node);
	}

	if (nodes)
		return (percent_sum) / (nodes);
	return 0;
}

/* a traversal calculation of completion level
*/
int node_calc_complete (Node *node)
{
	int percent_sum = 0;
	int nodes = 0;

	if (!(node_getflag (node, F_todo)))
		return -1;				/* node has no completion status */

	if (node_getflag (node, F_done))
		return 2000;			/* this node is done */

	if (node_getpercent_done (node) != -1)
		return node_getpercent_done (node) * 10;

	if (!node_right (node))
		return 0;				/* no undone children,.. completly undone */

	node = node_right (node);

	while (node) {
		switch (node_calc_complete (node)) {
			case -1:
				break;
			case 2000:
				percent_sum += 1000;
				nodes++;
				break;
			default:
				percent_sum += node_calc_complete (node);
				nodes++;
				break;
		}
		node = node_down (node);
	}

	if (nodes)
		return (percent_sum) / (nodes);
	return 0;
}

static int prefs_bullet_empty = 0;	/*could be made a preference, but I'm not sure */

/** 
	@return -1, no todo information, 0 not started, 1..1000 = 1..100% , 2000 = done */
int done_status (Node *a)
{
	int completion;

	if (prefs.showpercent) {
		completion = node_calc_complete (a);
	} else {
		if (node_getflag (a, F_todo)) {
			if (node_getflag (a, F_done)) {
				completion = 2000;
			} else {
				completion = 0;
			}
		} else {
			completion = -1;
		}
	}

	/* FIXME this is the wrong location for this code,.. must fixup the drawing code */
	if (!prefs_bullet_empty && completion == -1) {
		char *data = node_getdata (a);
		int j = 0;

		completion = -2;
		while (data[j]) {
			if (data[j++] != ' ') {
				completion = -1;
				break;
			}
		}
	}
	return completion;
}

int node_calc_size(Node *node){
	int size_sum = 0;
	int nodes = 0;

	if (!(node_getflag (node, F_todo)))
		return -1;

	if(node_getsize(node)!=-1)
		return node_getsize(node);

	if (!node_right (node)){ /* leaf node */
		if(node_getsize(node)!=-1)
			return node_getsize(node);
		else
			return 10; /* default size value */
	}

	node = node_right (node);

	while (node) {
		switch (node_calc_complete (node)) {
			case -1:
				break;
			default:
				size_sum += node_calc_size (node);
				nodes ++;
				break;
		}
		node = node_down (node);
	}

	if (nodes)
		return (size_sum);
	return 10; /* no of the children had any time,.. so we default to 1 */

}

#define def_size 1

/** 
    @param Node *node the node that we calculate for
	@param *retsize a parameter to put accumulated size in

	@return -1       no todo information,
			 0       not started, 
			 1..1000 1..100% , 
			 2000    done            percentage
*/

int calc_percentage_size(Node *node, int *retsize){
	int percentage=node_getpercent_done(node);
	int size=node_getsize(node);
	int size_todo=0;
	int size_done=0;
	int childnodes=0;
	Node *tnode;
	
	if (!(node_getflag (node, F_todo))){ /* bail out if not todo info set */
		*retsize=-1;
		return -1;
	}

	tnode=node_right(node);

	while(tnode){
		int rsize, rperc;
			
		rperc=calc_percentage_size(tnode,&rsize);
		switch(rperc){
			case -1:break;
			case 2000:
				if(rsize!=-1)size_todo+=rsize;
				size_done+=rsize*100;
				childnodes++;
				break;
			default:
				if(rsize!=-1)size_todo+=rsize;
				size_done+=rsize*rperc;
				childnodes++;
				break;
		}
		tnode=node_down(tnode);
	}

	if(!childnodes){
		size_todo=def_size;
		size_done=0;
	}

	if(size!=-1){
		if(childnodes)size+=size_todo;
	} else {
		size=size_todo;
	}
	*retsize=size;

	if(node_getflag(node,F_done))
		return 2000;
	
	if(percentage!=-1){
		return percentage;
	} else {
		percentage=size_done/size_todo;
	}
	
	return percentage;
}
