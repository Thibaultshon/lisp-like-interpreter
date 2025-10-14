
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>


#include "util.h"
#include "env.h"
#include "tokenize.h"
#include "parser.h"
#include "eval.h"

int main(){
  /* char input[] = "+++ \n"; */
  /* char input[] = "(+ 2 (+ 3 4))"; */
  /* char input[] = "(+ 2 3)"; */
  /* char input[] = "(/ 8 2  2 2)"; */
  /* char input[] = "(* 2 2  2 2)"; */
  /* char input[] = "(- 4 2)"; */
  /* char input[] = "(+ 5  (- 3 (* 1 1)))"; */
  /* char input[] = "(/ 8 2  2 (+ 5  (- 3 (* 1 1))))"; */
  /* char input[] = "(+ 25 35)"; */
  
  /* char input[] = "(if 0 1 2)"; */
  /* char input[] = "(if 1 1 2)"; */
  /* char input[] = "(if (- 1 1) 1 2)"; */
/* char input[] = "(if (- 2 1) 1 2)"; */

  /* char input[] = "(switch 3 (case 1 2) (case 3 4))"; */
  /* char input[] = "(switch 1 (case 1 2) (case 3 4))"; */
  /* char input[] = "(+ (:= x 4) 1)"; */
  /* char input[] = "(:= x 4)"; */

  //// to implement
  /* char input[] = "(:= x 4) (:= y 2)"; */
  /* char input[] = "(+ (:= x (:= y (+ 3 4))) y)"; */
  /* char input[] = "(while 0 (+ 1 2))"; */ // need assignment first




  printf("input:\n %s\n\n",input);

  /////Lex
  struct Token* tokens = lexer(input);
  printf("tokens:\n");
  for (int i = 0; tokens[i].type != END_LINE ;i++){
    printf("%s ", enumToString(tokens[i].type));

  }

  //////Parse
  printf("\n\n");
  struct Parser parser;
  parser.tokens = tokens;
  parser.curPos = 0;
  parser.ast = parse(&parser);
  printf("\n\nas cons:\n");
  printNode(parser.ast);
  printf("\n\n");


  /////Eval
  int result;
  int err = eval(parser.ast, &result);
  printf("error status: %d\n", err);
  printf("\nresult %d\n",result);

  freeTokens(parser.tokens);
  freeNode(parser.ast);
  freeEnv();

}










  








    
