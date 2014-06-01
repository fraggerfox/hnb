#ifndef CLI_H
#define CLI_H

/* execute specified command line as if we stood on pos in the tree

  Returns: new position, if position changes due to the action.
*/
Node *docmd(Node *pos,char * commandline);

/* enter an read eval loop executing commandlines
   
  Returns: new position, if position changes due to the actions done.
*/
Node *cli(Node *pos);

#endif /* CLI_H */
