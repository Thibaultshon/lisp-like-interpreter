#ifndef ENV_H
#define ENV_H

#include "uthash.h"


struct Binding {
  char *name;
  int val;
  UT_hash_handle hh;
};


struct EnvFrame {
  struct EnvFrame * parent;
  struct Binding * bindings;
};




void addIdentifier(struct EnvFrame* frame, struct Binding *identifier);

void deleteIdentifier(struct EnvFrame* frame,char* name);

struct Binding * findIdentifier(struct EnvFrame* frame ,char* name);

struct EnvFrame * enterEnv(struct EnvFrame* cur_env);

struct EnvFrame * leaveEnv(struct EnvFrame* cur_env);


#endif
