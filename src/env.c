#include <stddef.h>
#include <stdio.h>
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
    // todo - check if already in any scopes above including current scope
    struct Binding* new_var = malloc(sizeof(struct Binding)); 
    new_var->name = strdup(name);
    new_var->val  = val; //todo - assign ready set variables outside of current scope
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


void printEnvironments(struct EnvFrame* env){
  struct EnvFrame* cur_env =  env;
  struct Binding* tmp;
  printf("\n[");
  while (cur_env != NULL){
    printf("[");
    for(tmp=cur_env->bindings; tmp!=NULL; tmp= (struct Binding*)(tmp->hh.next)){
      printf("(%s = %d) ",tmp->name,tmp->val);
    }
    printf("] ");

    cur_env = cur_env->parent; //todo - reverse order

  }
  printf("]\n");
}





//todo - print enviroment and comp environmemtn functions using iteration macro
