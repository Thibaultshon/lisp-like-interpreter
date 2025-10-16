#include <stddef.h>
#include "env.h"

struct EnvFrame * enterEnv(struct EnvFrame* cur_env){
  struct EnvFrame* new_env  = malloc(sizeof(struct EnvFrame));
  new_env->bindings = NULL;
  new_env->parent = cur_env;
  return new_env;
}

struct EnvFrame * leaveEnv(struct EnvFrame* cur_env){
  struct EnvFrame* prev_env = cur_env;
  cur_env  = cur_env->parent;
  
  struct Binding *cur, *tmp; 
  HASH_ITER(hh,prev_env->bindings,cur, tmp){
    HASH_DEL(prev_env->bindings,cur);
    free(cur->name);
    free(cur);
  }
  free(prev_env);
  return cur_env;
}



void addIdentifier(struct EnvFrame* frame, struct Binding *identifier){
  HASH_ADD_STR(frame->bindings, name,identifier);

}


struct Binding *findIdentifier(struct EnvFrame* frame ,char* name){
  struct Binding *node;
  HASH_FIND_STR(frame->bindings,name,node);
  return node;
}




void deleteIdentifier(struct EnvFrame* frame,char* name){
  struct Binding* identifier = findIdentifier(frame,name);
  HASH_DEL(frame->bindings,identifier);
  if (identifier){
    free(identifier->name);
    free(identifier);
  }
}


