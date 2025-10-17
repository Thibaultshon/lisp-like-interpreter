#include <stddef.h>
#include "env.h"

struct EnvFrame * enterEnv(struct EnvFrame* cur_env){
  struct EnvFrame* new_env  = malloc(sizeof(struct EnvFrame));
  new_env->bindings = NULL;
  new_env->parent = cur_env;
  /* cur_env = new_env; */
  return new_env;
}

struct EnvFrame* leaveEnv(struct EnvFrame* cur_env){
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



void assign(struct EnvFrame* frame, char* name, int val){
    struct Binding* new_var = malloc(sizeof(struct Binding)); 
    new_var->name = strdup(name);
    new_var->val  = val;
    HASH_ADD_STR(frame->bindings, name,new_var);
}



struct Binding* deref(struct EnvFrame* frame ,char* name){
  struct Binding *node = NULL;
  struct EnvFrame* cur_frame = frame; // todo - return error if not found
  while (cur_frame != NULL && node== NULL){
    HASH_FIND_STR(cur_frame->bindings,name,node);
    cur_frame = cur_frame->parent;
  }
  return node;
}




void deleteIdentifier(struct EnvFrame* frame,char* name){
  struct Binding* identifier = deref(frame,name);
  HASH_DEL(frame->bindings,identifier);
  if (identifier){
    free(identifier->name);
    free(identifier);
  }
}


