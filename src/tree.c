/*
 * tree.c -- a general tree structure for use with hnb
 *
 * Copyright (C) 2001,2001 Øyvind Kolås <pippin@users.sourceforge.net>
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


/* there is one a little bit strange thing about
this tree, it have the root at the top but
at the left.. (like the model presented to the 
user) */

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"

Node *node_recurse (Node *node){
	if (node_right (node))
		return node_right (node);
	if (node_down (node))
		return node_down (node);
	
	while (node_left (node)) {
		if (node_down (node_left (node)))
			return node_down (node_left (node));
		node = node_left (node);
	}
	return 0;
}

Node *node_backrecurse (Node *node){
	if (node_up(node)){
		node=node_up(node);
		while(node_right(node)){
			node=node_right(node);
			node=node_bottom(node);
		}
		return(node);
	}
	return(node_left(node));
}

int node_no (Node *node){
	int no=0;
	while(node){
		node=node_backrecurse(node);
		no++;
	}
	return no;
}

Node *node_top (Node *node){
	if (node == 0)
		return 0;
	
	while (node_up (node))
		node = node_up (node);
	return (node);
}

Node *node_bottom (Node *node){
	if (node == 0)
		return 0;
	while (node_down (node))
		node = node_down (node);
	return node;
}

Node *node_insert_up (Node *node){
	Node *temp, *new = node_new ();
	
	temp = node->up;
	new->up = temp;
	new->down = node;
	
	node->up = new;
	if (temp)
		temp->down = new;
	
	new->left = node->left;
	
	if (node_left (new)) {		/* make tree consistent */
		if (node_right (node_left (node)) == node) {
			temp = node_left (new);
			temp->right = new;
		}
	}
	
	return new;
}

Node *node_insert_down (Node *node){
	Node *temp, *new = node_new ();
	
	temp = node->down;
	new->down = temp;
	new->up = node;
	
	node->down = new;
	if (temp)
		temp->up = new;
	
	new->left = node->left;
	
	return new;
}

Node *node_insert_right (Node *node){
	Node *new = node_new ();
	
	if ((!node) || (node->right)) {
		free (new);
		return 0;
	}
	
	new->left = node;
	node->right = new;
	
	return new;
}

unsigned int nodes_left (Node *node){
	unsigned int level = 0;
	
	while ((node = node_left (node)))
		level++;
	return level;
}

unsigned int nodes_up (Node *node){
	unsigned int level = 0;
	
	while ((node=node_up (node)))
		level++;
	return level;
}

unsigned int nodes_down (Node *node){
	unsigned int level = 0;
	
	while ((node=node_down (node)))
		level++;
	return level;
}

unsigned int nodes_right (Node *node){
	unsigned int level = 0;
	
	while ((node=node_right (node)))
		level++;
	return (level);
}

Node *node_remove (Node *node){
	Node *tup=node->up,*tdown=node->down;  
	
	/* if we're wiping the tree, add a temp node for later reference to the empty tree */
	if ((node_left (node)==0) && (node_up (node)==0) && (node_down (node)==0)) {
		Node *tnode=node_insert_down(node);
		node_setflag (tnode, F_temp,1 );
		tdown=node_down(node);
	}
	
	/* remove all children */
	while (node_right (node))
		node_remove (node_right (node));
	
	/* close the gap in the linked list */
	if (tup) 	tup->down = tdown;
	if (tdown)	tdown->up = tup;

	/* if we are a top-most child (parent says we are master of our siblings) */
	if ((node_left (node) ) && ( node_right(node_left (node) ) == node)) {
		if (tdown)	/* rearrange parents pointer */
			node->left->right = tdown;
		else { /* if no siblings remove ourselves, and return parent */
			Node *tnode = node_left (node);
	
		node->left->right = 0;
		node_free (node);
			return tnode;
		}
	}
	
	node_free (node);
	
	if (tup)	return tup;
	if (tdown)	return tdown;
	printf("we're not where we should be\n");
	return 0;
}

Node *node_match (char *match, Node *where){
	Node *node;
	
	node = node_top (where); /* do I want a match from top, or from where?*/
	if (!match[0])
		return 0;
	
	do{
		if (strncmp (node->data, match, strlen (match)) == 0)
			return node;
	} while((node = node_down(node)));
	
	return 0;
}

Node *node_exact_match (char *match, Node *where){
	Node *node;
	
	node = node_top (where); /* see node_match */
	if (!match[0])
		return 0;
	
	do{
		if (strcmp (node->data, match) == 0)
			return node;
	} while((node = node_down(node)));
	
	return 0;
}

/* this is a commodity funciton, and I didn't want to code it myself,.. I
searched the fine web, found, cut'd, 'n', pasted.. 
 url: http://www.brokersys.com/snippets/STRISTR.C
*/

/*
** Designation:  StriStr
**
** Call syntax:  char *stristr(char *String, char *Pattern)
**
** Description:  This function is an ANSI version of strstr() with
**               case insensitivity.
**
** Return item:  char *pointer if Pattern is found in String, else
**               pointer to 0
**
** Rev History:  07/04/95  Bob Stout  ANSI-fy
**               02/03/94  Fred Cole  Original
**
** Hereby donated to public domain.
*/

char *stristr (const char *String, const char *Pattern){
	char *pptr, *sptr, *start;
	int slen, plen;
	
	for (start = (char *) String,
		 pptr = (char *) Pattern, slen = strlen (String), plen = strlen (Pattern);	/* while string length not shorter than pattern length */
		 slen >= plen; start++, slen--) {
		/* find start of pattern in string */
		while (toupper (*start) != toupper (*Pattern)) {
			start++;
			slen--;		/* if pattern longer than string */
			if (slen < plen)
				return (NULL);
		}
		sptr = start;
		pptr = (char *) Pattern;
		while (toupper (*sptr) == toupper (*pptr)) {
			sptr++;
			pptr++;	/* if end of pattern then pattern was found */
			if ('\0' == *pptr)
				return (start);
		}
	}
	return (NULL);
}

/*returns the next recursive node having match as a substring, or 0 if not found
  starting from where.
*/

Node *node_recursive_match (char *match, Node *where){
	if (!match[0])
		return 0;
	
	where = node_recurse (where);	/* skip forward */
	while (where) {
		if (stristr (where->data, match) != NULL)	/* case insensitive */
			return where;
		where = node_recurse (where);
	}
	
	return 0;
}

Node *tree_new (){
	Node *root;
	
	root = node_new ();
	node_setflags (root, F_temp);
	return root;
}

Node *node_root (Node *node){
	while (node_left (node))
		node = node_left (node);
	node = node_top (node);
	return node;
}

void tree_free (Node *node){
	Node *root = node_root (node);
	
	while (node_down (root))
		node_remove (node_down (root));
	
	root=node_remove(root);
	node_free(root);
	
	return;
}

#define swp(a,b,t)	t=a;a=b;b=t;

/*  swaps all the data and children of the two specified node structures */
/*  possible conflicts here:  what if swapping a child with it's parent? */

void node_swap (Node *nodeA, Node *nodeB){
	Node *tnode;
	int tint;
	char *tstr;
	
	swp (nodeA->right, nodeB->right, tnode);
	swp (nodeA->flags, nodeB->flags, tint);
	swp (nodeA->priority, nodeB->priority, tint);
	swp (nodeA->data, nodeB->data, tstr);
	
	tnode = node_right (nodeA);
	while (tnode) {
		tnode->left = nodeA;
		tnode = node_down (tnode);
	}

	tnode = node_right (nodeB);
	while (tnode) {
		tnode->left = nodeB;
		tnode = node_down (tnode);
	}
}

void node_update_parents_todo (Node *pos){
	Node *tnode = node_top (pos);
	
	if (node_left (pos) && node_getflag (node_left (pos), F_todo)
			&& !node_getflag (pos,F_temp)) {

		int status = -1;

		while (tnode && status) {
			if (node_getflag (tnode , F_todo)){
				status = 1;
				if ( !node_getflag (tnode, F_done) )
					status = 0;
			}
			tnode = node_down (tnode);
		}

		tnode = node_left (pos);

		if (status == 1){		/* all todo nodes were checked */
			node_setflag(tnode,F_done,1);
			node_update_parents_todo (tnode);						
		} else if (status == 0){	/* one or more todo node were unchecked*/
			node_setflag(tnode,F_done,0);
			node_update_parents_todo (tnode);
		}
	}
}

int cmp_ascdata(Node *a,Node *b){
	return(strcmp(a->data,b->data));
}

int cmp_descdata(Node *a,Node *b){
	return(strcmp(b->data,a->data));
}

int cmp_todo(Node *a,Node *b){
	if( !a->flags && !b->flags )	/* accessing data directly for speed, */
		return(strcmp(a->data,b->data));	

	if((a->flags&F_todo) <(b->flags&F_todo))return 1; /*  all todos at top */
	if((a->flags&F_todo) >(b->flags&F_todo))return -1;	
	if((a->flags&F_todo)==(b->flags&F_todo)){
		if(a->flags&F_todo){
			if((a->flags&F_done) >(b->flags&F_done))return 1;/* not done*/
			if((a->flags&F_done) <(b->flags&F_done))return -1; 
			
			if((a->priority) <(b->priority))return 1;	/*  priority  */
			if((a->priority) >(b->priority))return -1;	
			
		}
	}
	return(strcmp(a->data,b->data));
}

/* this quicksort costs less cputime, but costed a hell of a lot more braintime
   than the bubble sort */
Node *quicksort(Node *Top, Node *Bottom, int (*cmp) (Node *a, Node *b) )
{
	Node *part, *j;
	if( Bottom!=Top && (Bottom != Top && node_down(Bottom)!=Top) ){
		j=node_up(Bottom);
		
		if(j==Top){
			if(cmp( Top, Bottom ) >=0)
				node_swap( Top, Bottom);
		} else {
			part=Top;		
			while(part!=j){	
				while( cmp( part, Bottom ) <0)
					part=node_down(part);
				while( j && j!=part && cmp( j, Bottom ) >= 0 )
					j=node_up(j);
				if(!j || node_up(part) == j )
					break;
				node_swap( part, j);
			}
			if( part != Bottom )
				node_swap( part, Bottom);
			
			quicksort( Top, node_up(part),cmp ); /* Sort  upper part */
			if(node_down(part))
				quicksort( node_down(part), Bottom,cmp ); /* sort lower part */
		}
	}
	return(Top);
}

Node *node_sort_siblings (Node *node){
	return(quicksort( node_top(node), node_bottom(node),cmp_todo));
}

/*	returns an number between 0 and 1000 showing the completion of this node,
	computed as follows:
		all children with todo boxes count as one part
		if a child is done count+= 1000/parts
		if a child is not done count+= node_calc_complete(child)/parts	
*/

int node_calc_complete (Node *node){
	Node *tnode;
	int count = 0, parts = 0;
	
	if (!(node_getflag (node, F_todo)))
		return -1;				/* node has no completion status */
	if (node_getflag (node, F_done))
		return 1000;			/* this node is done */
	if (!node_right (node))
		return 0;			/* no undone children,.. completly undone */
	
	tnode = node_right (node);
	while (tnode) {
		if (node_getflag (tnode, F_todo))
			parts++;
		tnode = node_down (tnode);
	}
      if(!parts)
		return 0;			/* division by zero is fun */
	tnode = node_right (node);
	
	while (tnode) {
		if (node_getflag (tnode, F_todo)) {
			if (node_getflag (tnode, F_done)) {
				count += 1000;
			} else {
				count += node_calc_complete (tnode);
			}
		}
		tnode = node_down (tnode);
	}
	return count / parts;
}

Node *node_lower(Node *node){
	if (node_left (node)) {
		Node *tnode,*bnode;
	
	bnode=node_bottom(node);
	while (node_down (node)) {
		tnode = node_insert_down (node_left (node));
			node_swap (tnode, bnode);
			bnode=node_remove (bnode);
		}
		tnode = node_insert_down (node_left (node));
		node_swap (tnode, bnode);
		node_remove (bnode);
		return(tnode);
	}
	return(node);
}

Node *node_raise(Node *node){
	if (node_up (node)) {
		Node *tnode, *first_moved;
	
	node = node_up (node);	/* go up  */
	
	if (!(tnode = node_right (node))) {	/* must install a child */
		tnode = node_insert_right (node);
		} else {
			tnode = node_insert_down (node_bottom (tnode));
		}
		
		node_swap (node_down (node), tnode);
		node_remove (node_down (node));
		first_moved = tnode;
		while (node_down (node)) {
			tnode =	node_insert_down (tnode);
			node_swap (node_down (node), tnode);
			node_remove (node_down (node));
		}
		return first_moved;
	}
	return node;
}
