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
    int prev;
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

      case EQ:
      case NEQ:
      case GT:
      case LT:
        node = CDR(node);

        evalSExpression(CAR(node), &prev);
        *res = 1;

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
      switch (op)
        {
        case ADD: *res = *res + val; break;
        case SUB: *res = *res - val; break;
        case MUL: *res = *res * val; break;
        case DIV:  *res = *res / val; break;


        case NEQ:  *res = *res && prev != val; prev = val;break; // todo - way to finish early
        case EQ:  *res = *res && prev == val; prev = val;break;
        case GT:  *res = *res && prev > val; prev = val;break;
        case LT: *res = *res && prev < val; prev = val; break;
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




int evalIF(struct Node* node, int* res){
  int pred_val;                      
  if (evalSExpression(CAR(node), &pred_val) == 1) return 1;

  struct Node* branch = CDR(node);
  struct Node* seq = CAR(branch); 
  struct Node* alt = CAR(CDR(branch)); // todo - error check
  
  if (pred_val != 0){
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
  if (evalSExpression(CAR(node), &pred_val) == 1) return 1;

  struct Node* init_seq =  CDR(node);

  while (pred_val != 0){
  struct Node* cur_seq  = init_seq;
    while (cur_seq != NULL){
      evalSExpression(CAR(cur_seq),res);  
      cur_seq = CDR(cur_seq);
    }
    if (evalSExpression(CAR(node), &pred_val) == 1) return 1;
  }
  return 0;
}



int evalAssign(struct Node* node, int* res){
  char* name = CAR(node)->atom.name; //todo - check if identifier type
  
  int value;
  evalSExpression(CAR(CDR(node)),&value);

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
  // todo - backup reset res to something if not set
  return evalSExpression(node,res);
}




  
  

