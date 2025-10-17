#ifndef EVAL_H
#define EVAL_H

#include <stdbool.h>
#include "parser.h"
#include "env.h"


int eval(struct Node* node, int* res);
extern struct EnvFrame *g_env;


enum valType{
  INT_TYPE,
  LIST_TYPE
};

struct Result {
  enum valType type;
  union {
    struct Node* list_val;
    int int_val;
  };
};




#endif
