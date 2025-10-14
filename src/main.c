
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

  //// to implement
  char input[] = "(:= x 3)";
  /* char input[] = "(:= x 4) (:= y 2)"; */
  /* char input[] = "(+ (:= x (:= y (+ 3 4))) y)"; */
  /* char input[] = "(while 0 (+ 1 2))"; */ // need assignment first


  printf("\ninput:\n%s\n\n",input);

  /////Lex
  struct Token* tokens = lexer(input);
  printf("tokens:\n");
  for (int i = 0; tokens[i].type != END_LINE ;i++){
    printf("%s ", enumToString(tokens[i].type));

  }

  //////Parse
  printf("\n\nParser:\n");
  struct Parser parser;
  parser.tokens = tokens;
  parser.curPos = 0;
  parser.ast = parse(&parser);
  printNode(parser.ast);
  printf("\n");


  /////Eval
  printf("\nSemantics:\n");
  int result;
  int err = eval(parser.ast, &result);
  printf("error status: %d\n", err);
  printf("result: %d\n",result);

  freeTokens(parser.tokens);
  freeNode(parser.ast);
  freeEnv();

}










  








    
