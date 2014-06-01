#ifndef FILE_H
#define FILE_H

#if HAVE_LIBXML2==1 && HAVE_LIBXML_PARSER_H==1 && HAVE_LIBXML_TREE_H==1
#define USE_LIBXML
#endif

/* store node importer state
*/
typedef struct
{
	Node	*npos;
	int	startlevel;
} import_state_t;

/* initialize importer, tree will be attached under *node
*/
extern void init_import(import_state_t *is, Node *node);

/* import node into tree
*/
extern void import_node (import_state_t *is, int level,
		int flags, int priority, char *data);

/* imports file *filename below specified node, if tree was empty
   return first node imported.
*/
Node *ascii_import(Node *node,char *filename);

/* exports recursively starting on specified node, to the file *filename
*/
void ascii_export(Node *node,char *filename);

/* loads the tutorial into the tree
*/
Node *help_import(Node *node);

/* exports recursively starting on specified node, to the file *filename
*/
void help_export(Node *node,char *filename);


int xml_check(char *filename);

/*returns 1 if file exists*/
int file_check(char *filename);

/* imports file *filename below specified node, if tree was empty
   return first node imported.
*/
Node *hnb_import (Node *node, char *filename);

/* exports recursively starting on specified node, to the file *filename
*/
void hnb_export (Node *node, char *filename);

/* exports recursively starting on specified node, to the file *filename
*/
void html_export(Node *node,char *filename);

Node *xml_import (Node *node, char *filename);
void xml_export (Node *node, char *filename);

#ifdef USE_LIBXML
void libxml_export(Node *node,char *filename);
Node *libxml_import(Node *node,char *filename);
#endif

extern void (* ptr_export)(Node *,char *);
extern Node * (* ptr_import)(Node *,char *);

#endif /* FILE_H*/
