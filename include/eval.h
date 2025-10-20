#ifndef EVAL_H
#define EVAL_H

#include "parser.h"
#include "env.h"


int eval(struct Node* node, int* res);
extern struct EnvFrame* g_env;



struct Result {
  enum NodeType type;
  union { // todo - name it val?
    struct Node* list_val;
    int int_val;
    char* symbol_val;
  };
};




#endif
