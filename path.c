#include "node.h"
#include "tree.h"

char path_seperator = '/';

Node *
path2node (char *path)
{
  tnode = get_root;

  while gettoken
    {
      tnode = match (token);
    };


  return (tnode);
};

char *
node2path (Node * node)
{
  int levels = nodes_left (node);
  int cnt;


  append (path_seperator);

  for (cnt = levels; cnt > 0; cnt--)
    {
      int cnt2 tnode = node;
      for (cnt2 = 0; cnt2 < cnt; cnt++);
      tnode = node_left (tnode);
      append (tnode_data);
      append (path_seperator);
    };

  return string;
};
