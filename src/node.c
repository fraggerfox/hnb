/*
 * node.c -- definition of hnb's nodes
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"

void node_setdata (Node *node, char *data){
	free (node->data);
	node->data = (char *) strdup (data);
}

char *node_getdata (Node *node){
	if (node == NULL)
		return ("");
	return node->data;
}

Node *node_new (){
	Node *node = (Node *) malloc (sizeof (Node));
	
	node->up = 0;
	node->down = 0;
	node->right = 0;
	node->left = 0;
	node->data = (char *) strdup ("");
	node->flags = 0;
	node->priority = 0;
	return node;
}

void node_free (Node *node){
	free (node->data);
	free (node);
}

unsigned char node_getpriority(Node *node){
	return(node->priority);
}
void node_setpriority(Node *node, unsigned char priority){
	node->priority=priority;
}

void node_setflag(Node *node,int flag,int state){
	if(state){
		if (!node_getflag(node,flag))
			node_setflags(node,node_getflags(node)+flag);
	} else {
		if (node_getflag(node,flag))
			node_setflags(node,node_getflags(node)-flag);
	}
}

int node_toggleflag(Node *node, int flag){
	if (node_getflag(node,flag)) {
		node_setflag(node,flag,0);
		return 0;
	} else {
		node_setflag(node,flag,1);
		return 1;		
	}
}
