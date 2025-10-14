#include <stddef.h>
#include "env.h"

struct Hash_Node *g_env = NULL;

void addIdentifier(struct Hash_Node *identifier){
  HASH_ADD_STR(g_env, name,identifier);

}

struct Hash_Node *findIdentifier(char* name){
  struct Hash_Node *node;
  HASH_FIND_STR(g_env,name,node);
  return node;
}




void deleteIdentifier(char* name){
  struct Hash_Node* identifier = findIdentifier(name);
  HASH_DEL(g_env,identifier);
  if (identifier){
    free(identifier->name);
    free(identifier);

  }

}


void freeEnv(){
  struct Hash_Node *cur, *tmp;              
  HASH_ITER(hh,g_env,cur, tmp){             
    HASH_DEL(g_env,cur);                    
    free(cur->name);                        
    free(cur);                              
  }                                         
}                                           
