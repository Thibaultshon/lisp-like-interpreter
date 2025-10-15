
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

int interpret(char* input,int* res){
  struct Token* tokens;
  struct Parser parser;
  tokens= lexer(input);
  parser.tokens = tokens;
  parser.curPos = 0;

  while (peak(&parser).type != END_LINE){
    parser.ast = parse(&parser);
    int status = eval(parser.ast, res);
    if (status !=0){
      fprintf(stderr,"\nSemantic Error\n");
      return status;
    }else{
      printf("%d\n",*res);
    }
  }
  freeTokens(parser.tokens);
  freeNode(parser.ast);

  return 0;
}

int repl(){
  printf("Starting REPL:\n");
  printf("Use exit() to exit the program \n");
  int res;
  while(1){
    printf(">> ");
    char input[1024];
    fgets(input,sizeof(input),stdin);
    input[strcspn(input, "\n")] = 0;
    if (strcmp(input,"exit()") ==0){
        break;
    }
    if (interpret(input,&res)!=0){
      return 1;
    }

  }
  freeEnv();
  return 0;
}



int main(){
  return repl();

}










  








    
