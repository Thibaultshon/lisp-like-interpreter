
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
  parser.ast = parse(&parser);
  int error = eval(parser.ast, res);
  freeTokens(parser.tokens);
  freeNode(parser.ast);
  freeEnv();
  return error;
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
    int status = interpret(input,&res);
    if (status !=0){
      fprintf(stderr,"\nSemantic Error\n");
    }else{
      printf("%d\n",res);
    }
      
  }
  return 0;
}



int main(){
  return repl();

}










  








    
