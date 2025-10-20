#ifndef EVAL_H
#define EVAL_H

#include "parser.h"
#include "env.h"





struct Result {
  enum NodeType type; //todo - use on types and remove lambda type from ast
  union { // todo - name it val?
    struct Node* list_val;
    int int_val;
    char* symbol_val;
  };
};

int eval(struct Node* node, struct Result* res);

void printResult(struct Result* result);
char* resultGetSymbol(struct Result* res);
char* resultGetKeyword(struct Result* res);
struct Node* resultGetLambda(struct Result* res);
int resultGetInt(struct Result* res);

extern struct EnvFrame* g_env;


#endif
