#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "util.h"
#include "env.h"
#include "tokenize.h"

#include "interpreter.h"



void registerNodes(struct NodeRegistry* registry, struct Node* nodes){ //tmp until add garbage collector or change lambda works
    registry->nodes[registry->count] = nodes;
    registry->count++;
    if (registry->count > registry->max){
      registry->max +=10; 
      registry->nodes = realloc(registry->nodes,registry->max*sizeof(**registry->nodes));
    }
}


void freeRegistry(struct NodeRegistry* registry){
  for (int i =0; i <registry->count;i++){
    freeNode(registry->nodes[i]);
  }
  free(registry->nodes);
}



int interpret(char* input,struct Result* res,struct NodeRegistry* registry,bool repl_mode){
  struct Parser parser;
  initParser(&parser);

  
  while (peek(&parser,input).type != TOK_END_LINE){
    struct Node* ast = NULL;
    enum ParseResultType status = parse(&parser,input,&ast);

    if (status == PARSE_RESULT_SUCCESS){
      eval(ast, res);
    } else{
      //parsing error
      res->type = RESULT_ERROR;
      res->symbol_val = (char*)parseTypeToString(status); //todo - use enumtype for error nto char
    }

    if (repl_mode){
      printResult(res);
      printf("\n");
    }


    registerNodes(registry,ast);
    if (res->type == RESULT_ERROR) break;
  }
  freeParser(&parser);


  return 0;
}
