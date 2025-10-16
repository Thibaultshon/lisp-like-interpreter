
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
  struct Parser parser;
  initParser(&parser);
  while (peek(&parser,input).type != END_LINE){
    struct Node* ast = parse(&parser,input);
    int status = eval(ast, res);
    if (status !=0){
      fprintf(stderr,"\nSemantic Error\n");
      return status;
    }else{
      printf("%d\n",*res);
    }

    freeNode(ast);
  }
  freeParser(&parser);


  return 0;
}

int repl(){
  printf("Starting REPL:\n");
  printf("Use exit() to exit the program \n");
  int res;
  g_env = enterEnv(NULL);
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
  leaveEnv(g_env);
  return 0;
}



int main(){
  return repl();

}










  








    
