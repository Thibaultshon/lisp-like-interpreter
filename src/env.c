#include <stddef.h>
#include <stdio.h>
#include "env.h"
#include "eval.h"


struct EnvFrame * enterEnv(struct EnvFrame* cur_env){
  struct EnvFrame* new_env  = malloc(sizeof(*new_env));
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
    free(cur->res); 
    free(cur);
  }
  free(prev_env);
  return cur_env;
}


void assign(struct EnvFrame* frame, char* name, struct Result* res){
    // todo - check if already in any scopes above including current scope
    struct Binding* new_var = malloc(sizeof(struct Binding)); 
    new_var->name = strdup(name);
    new_var->res  = res; //todo - assign ready set variables outside of current scope
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
    free(identifier->res);
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
      
      switch(tmp->res->type)
        {
        case NODE_NUM:
          printf("(%s = %d) ",tmp->name,tmp->res->int_val);
          break;
        case NODE_LAMBDA:
          printf("(%s = ",tmp->name);
          /* printNode(tmp->res->list_val); */
          printf(") ");
          break;
        case NODE_KEYWORD:
        case NODE_SYMBOL:
          printf("(%s = %s) ",tmp->name,tmp->res->symbol_val);
          break;
        }
    }
    printf("] ");

    cur_env = cur_env->parent; //todo - reverse order

  }
  printf("]\n");
}


//todo - print enviroment and comp environmemtn functions using iteration macro
