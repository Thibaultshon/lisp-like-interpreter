
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

struct Node** used_nodes; //todo - temp just for now
size_t node_count =0;
int max_nodes = 10;

int interpret(char* input,struct Result* res){
  struct Parser parser;
  initParser(&parser);

  
  while (peek(&parser,input).type != TOK_END_LINE){
    struct Node* ast = parse(&parser,input);
    int status = eval(ast, res);
    if (status !=0){
      fprintf(stderr,"\nSemantic Error\n");
      return status;
    }else{
      printResult(res);
      printf("\n");
    }

    used_nodes[node_count] = ast;
    node_count++;
    if (node_count > max_nodes){
      max_nodes +=10; 
      used_nodes = realloc(used_nodes,max_nodes*sizeof(**used_nodes));
    }
  }
  freeParser(&parser);


  return 0;
}

int repl(){
  printf("Starting REPL:\n");
  printf("Use exit() to exit the program \n");
  struct Result res;
  g_env = enterEnv(NULL);

  max_nodes = 10;
  node_count =0; 
  used_nodes = malloc(max_nodes*sizeof(*used_nodes));
  
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

  for (int i =0; i <node_count;i++){
    freeNode(used_nodes[i]);
  }
  free(used_nodes);
  leaveEnv(g_env);
  return 0;
}



int main(){
  return repl();

}










  








    
