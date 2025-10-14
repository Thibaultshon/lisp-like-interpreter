#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  
#include <stdbool.h>
#include <string.h> 


#include "util.h"
#include "tokenize.h"
#include "parser.h"
#include "env.h"
#include "eval.h"


int evalPredicate(struct Node* node, int* res);
int evalIF(struct Node* node, int* res);
int evalSwitch(struct Node* node, int* res);
int evalWhile(struct Node* node, int* res);
int evalAssign(struct Node* node, int* res);
int evalSExpression(struct Node* node, int* res);



int evalSExpression(struct Node* node, int* res){
  if (node == NULL){
    return 1;  
  } 
  if (!node->is_atom){
    enum TokenType op = CAR(node)->atom.type;

    switch(op)
      {
      case IF:
        return evalIF(CDR(node), res);
      case WHILE:
        return evalWhile(CDR(node), res);
      case SWITCH:
        return evalSwitch(CDR(node),res);
      case ASSIGN:
        return evalAssign(CDR(node),res);
      case ADD:
        *res = 0; break;
      case MUL:
        *res = 1; break;
      case DIV:
      case SUB:
        node = CDR(node);
        evalSExpression(CAR(node), res);
        break;
      default: return 1;

      }

    node = CDR(node);
    while(node){      
      int val;
      evalSExpression(CAR(node), &val);
      printf("%d",val);
      printf("%d\n",*res);

      switch (op)
        {
        case ADD: *res = *res + val; break;
        case SUB: *res = *res - val; break;
        case MUL: *res = *res * val; break;
        case DIV:  *res = *res / val; break;
        default: break;
        }


      node = CDR(node);
    }
  }else{
    enum TokenType type = node->atom.type;
    if (type == IDENTIFIER){
      struct Hash_Node* var = findIdentifier(node->atom.name);
      *res = var->val;
    }else{
      *res= node->atom.val; 
    }

  }
  return 0;
}



int evalPredicate(struct Node* node, int* res){
  struct Node* predictate = CAR(node);
  if (!predictate->is_atom){
    evalSExpression(predictate,res);
  }else{
    *res = predictate->atom.val;
  }
  return 0;
}


int evalIF(struct Node* node, int* res){
  int pred_val;                      
  if (evalPredicate(node,&pred_val) == 1) return 1;

  struct Node* branch = CDR(node);
  struct Node* seq = CAR(branch); 
  struct Node* alt = CAR(CDR(branch)); // todo - error check
  
  if (pred_val == 0){
    return evalSExpression(seq,res);

  }else{
    return  evalSExpression(alt,res);
  }
  
}

int evalSwitch(struct Node* node, int* res){
  int switch_val;
  evalSExpression(CAR(node),&switch_val);
  node = CDR(node);
  
  while (node != NULL){ // todo - or default case
    struct Node* cur_case = CAR(CDR(CAR(node))); // as first child will be keyword case
    struct Node* case_expr = CAR(CDR(CDR(CAR(node))));

    int case_val;
    evalSExpression(cur_case,&case_val);
    
    if (case_val == switch_val){
      return evalSExpression(case_expr, res);
    }
    node= CDR(node);
  }
  return 1;
}



int evalWhile(struct Node* node, int* res){
  int pred_val;                      
  if (evalPredicate(CAR(node), &pred_val) == 1) return 1;
  struct Node* seq =  CAR(node);

  while (pred_val == 0){
    printf("pre_val: %d \n", pred_val);
    evalSExpression(seq,res);
    if (evalPredicate(CAR(node), &pred_val) == 1) return 1;
  }
  return 0;
}



int evalAssign(struct Node* node, int* res){
  char* name = CAR(node)->atom.name; //todo - check if identifier type
  struct Node* value_node = CAR(CDR(node));
  int value = value_node->atom.val;
  struct Hash_Node* var = findIdentifier(name);
  if (var == NULL){
    struct Hash_Node* new_var = malloc(sizeof(struct Hash_Node));
    new_var->name = strdup(name);
    new_var->val  = value;
    addIdentifier(new_var);
  }else{
    var->val = value;
  }
  *res = value;
  return 0;

}






int eval(struct Node* node, int* res){
  printf("semantics:\n");
  return evalSExpression(node, res);
}



  
  

