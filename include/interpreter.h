#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "eval.h"
#include "parser.h"
struct NodeRegistry  {
  struct Node** nodes;
  size_t count;
  int max;
};

void registerNodes(struct NodeRegistry* registry, struct Node* node);
void freeRegistry(struct NodeRegistry* registry);
int interpret(char* input,struct Result* res,struct NodeRegistry* registry,bool repl_mode);



#endif
