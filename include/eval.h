#ifndef EVAL_H
#define EVAL_H

#include "parser.h"
#include "env.h"

enum ResultType {
  RESULT_LAMBDA,
  RESULT_INT,
  RESULT_SYMBOL,
  RESULT_STRING,
  RESULT_ERROR
};



struct Result {
  enum ResultType type; 
  union { 
    struct Node* list_val;
    int int_val;
    char* symbol_val;
  };
};

int eval(struct Node* node, struct Result* res);

void printResult(struct Result* result);
char* resultGetSymbol(struct Result* res);
struct Node* resultGetLambda(struct Result* res);
int resultGetInt(struct Result* res);
char* resultGetError(struct Result* res);

extern struct EnvFrame* g_env;


#endif
