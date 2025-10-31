
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include<stdbool.h>
#include "env.h"


#include "interpreter.h"





int repl(){
  printf("Starting REPL:\n");
  printf("Use exit() to exit the program \n");
  struct Result res;
  g_env = enterEnv(NULL);

  struct NodeRegistry registry = {
    .max = 10,
    .count = 0,
    .nodes = malloc(10 * sizeof(struct Node*))
  };

  while(1){
    printf(">> ");
    char input[1024];
    fgets(input,sizeof(input),stdin);
    input[strcspn(input, "\n")] = 0;
    if (strcmp(input,"exit()") ==0){
        break;
    }
    if (interpret(input,&res,&registry,true)!=0){
      return 1;
    }

  }

  freeRegistry(&registry);
  leaveEnv(g_env);
  return 0;
}



int main(){
  return repl();

}









  








    
