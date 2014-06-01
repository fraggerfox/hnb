#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "tree.h"
#include "file.h"
#include "path.h"

int main (int argc, char **argv)
{
  Node *pos;
  char path[512];
  path[0]=0;

  if(argc==2){
  	strcpy(path,argv[1]);
  } else {
    strcpy(path,"/");
  };
  printf("Content-type: text/html\n\n");
  
	tree_init();
	pos=tree_root();
	pos=ascii_import(pos,"README");
	
	if(no_path2node(path)!=0)
		pos=no_path2node(path);

	printf(
"<html>\n\
<head><title>Hierarchic Notebook homepage</title>\n\
\n\
<style type=text/css>a:link {text-decoration:none; color: #002288;}\n\
a:active {text-decoration:none; color: #002288;}\n\
a:visited {text-decoration:none; color: #002288;}\n\
td {font-family: verdana, Helvetica, sans-serif}\n\
th {font-family: verdana, Helvetica, sans-serif; background: #6688AA; color: #ffffff;}</style>\n\
\n\
</head>\n\
<body text=\"#330000\" bgcolor=\"#ffffff\">\n\
\n\
<table bgcolor=\"#6688AA\" border=0 width=100%% cellpadding=8 cellspacing=0 height=95%%>\n\
  <tr>\n\
    <td colspan=2 height=10%%>\n\
      <font color=\"#ffffff\" size=+3>&nbsp;Hierarchical Notebook</font>\n\
    </td>\n\
  </tr>\n\
  <tr>\n\
    <td valign=top width=20%%>\n\
\n\
    <!---- path ----->\n");

	{/* print path */
		int levels=nodes_left(pos);
		int cnt;
		printf("<br>\n&nbsp;<a href=\"%s?/\">/</a><br>\n",argv[0]);

		for(cnt = levels; cnt>0 ; cnt--){
			int cnt2;
			Node *tnode=pos;
			
			for(cnt2 = 0; cnt2 <cnt; cnt2++)
				tnode=node_left(tnode);

			printf("&nbsp;<a href=\"%s?%s\">",argv[0],node2no_path(node_right(tnode)));
			printf("%s", node_getdata(tnode));
			printf("</a>/<br>\n");
		};
		
		printf("\n");
	};
	printf(
"   </td>\n\
    <td valign=top width=80%% bgcolor=\"#ffffff\">\n\
    <!----- data -------->");

	pos=node_top(pos);
	do{

		if(node_right(pos)) /* we should make a link */
			printf(" &nbsp;<a href=\"%s?%s\">",argv[0],node2no_path(node_right(pos)));

		printf("%s",node_getdata(pos));
	
		if(node_right(pos)) /* we should make a link */
			printf("&nbsp;-=&gt;</a>");

		
	printf("<br>\n");
	
	} while( (pos=node_down(pos)) );

	printf(
"   </td>\n\
  </tr>\n\
  <tr>\n\
    <td valign=bottom>\n\
      <center><A href=\"http://sourceforge.net/projects/hnb/\">\n\
      <IMG src=\"http://sourceforge.net/sflogo.php?group_id=10100&type=1\" width=\"88\" height=\"31\" border=\"0\" alt=\"SourceForge Logo\"></A></center>\n\
    </td>\n\
    <td bgcolor=\"#ffffff\"></td>\n\
  </tr>\n\
</table>\n\
</body></html>");
		
	tree_free();
  return(0);
}
