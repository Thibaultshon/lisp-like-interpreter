#ifndef ENV_H
#define ENV_H

#include "uthash.h"


struct Binding {
  char *name;
  struct Result* res; 
  UT_hash_handle hh;
};


struct EnvFrame {
  struct EnvFrame * parent;
  struct Binding * bindings;
};



void assign(struct EnvFrame* frame, char* name, struct Result* res); //todo - maybe spereate functions as well assign int etc;

void deleteIdentifier(struct EnvFrame* frame,char* name);

struct Binding * deref(struct EnvFrame* frame ,char* name);

struct EnvFrame * enterEnv(struct EnvFrame* cur_env);

struct EnvFrame * leaveEnv(struct EnvFrame* cur_env);

void printEnvironments(struct EnvFrame* g_env);
#endif
