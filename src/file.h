#ifndef FILE_H
#define FILE_H

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

/* imports file *filename below specified node, if tree was empty
   return first node imported.
*/
Node *xml_import (Node *node, char *filename);

/* exports recursively starting on specified node, to the file *filename
*/
void xml_export (Node *node, char *filename);

/* exports recursively starting on specified node, to the file *filename
*/
void html_export(Node *node,char *filename);

Node *gxml_import (Node *node, char *filename);
void gxml_export (Node *node, char *filename);

#endif /* FILE_H*/
