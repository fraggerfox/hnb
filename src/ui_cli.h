#ifndef CLI_H
#define CLI_H

#include "cli.h"

/*	Initialize ui_cli module */
void init_ui_cli (void);


/** execute specified command line as if we stood on pos in the tree

  @returns new position, if position changes due to the action.
*/

Node *docmd (Node *pos, const char *commandline);


/* enter an read eval loop executing commandlines
   
  Returns: new position, if position changes due to the actions done.
*/
Node *cli (Node *pos);

#define docmdf(pos,args...)  \
     do{        char docmdf_buf[100];\
                snprintf (docmdf_buf, 99, args);\
                docmd(pos,docmdf_buf);\
       }while(0)



#endif /* CLI_H */
