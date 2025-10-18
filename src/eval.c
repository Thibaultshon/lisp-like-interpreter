#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  

#include <string.h> 
#include "util.h"
#include "tokenize.h"
#include "eval.h"

int eval(struct Node* node, int* res);
int evalSExpression(struct Node* node, struct Result* res);
int evalIF(struct Node* node, struct Result* res);
int evalSwitch(struct Node* node, struct Result* res);
int evalWhile(struct Node* node, struct Result* res);
int evalAssign(struct Node* node, struct Result* res);
int evalSeq(struct Node* seq, struct Result* res);
int evalLet(struct Node* node, struct Result* res);
int evalCallLambda(struct Node* eval_lambda_node, struct Result* res);
int evalSeq(struct Node* seq, struct Result* res);
int evalDefFunction(struct Node* seq, struct Result* res);

void resultSetInt(struct Result* res, int value);
int resultGetInt(struct Result* res);
void resultSetList(struct Result* res, struct Node* list);
struct Node* resultGetList(struct Result* res);

struct EnvFrame* g_env = NULL;


int eval(struct Node* node, int* res){
  struct Result new_res;
  resultSetInt(&new_res,0);
  int status = evalSExpression(node,&new_res); //todo - change to just return and receive result
  *res = resultGetInt(&new_res);
  return status;
}


int evalSExpression(struct Node* node, struct Result* res){
  if (node == NULL){
    return 1;  
  } 
  if (!node->is_atom){
    enum TokenType op = CAR(node)->atom.type;
    int prev;
    switch(op)
      {
      case IF:
        return evalIF(node, res); //todo - just pass node and do cdr within the actual function
      case WHILE:
        return evalWhile(node, res);
      case SWITCH:
        return evalSwitch(node,res);
      case ASSIGN:
        return evalAssign(node,res);
      case LET:
        return evalLet(node,res);
      case CALL:
        return evalCallLambda(node,res); // todo -later it can evaluate any s expression then how to evalambda
      case ADD:
        resultSetInt(res,0); break;

      case EQ:
      case NEQ:
      case GT:
      case LT:
        node = CDR(node);
        struct Result prev_res;
        evalSExpression(CAR(node), &prev_res);
        prev = resultGetInt(&prev_res);
      case MUL:
        resultSetInt(res,1); break;


      case DIV:
      case SUB:
        node = CDR(node);
        evalSExpression(CAR(node), res);
        break;
      default: return 1;

      }

    node = CDR(node);
    while(node){      
      struct Result val_result;
      evalSExpression(CAR(node), &val_result);
      int val = resultGetInt(&val_result);
      switch (op)
        {
        case ADD: resultSetInt(res,resultGetInt(res) + val); break;
        case SUB: resultSetInt(res,resultGetInt(res) - val); break;
        case MUL: resultSetInt(res,resultGetInt(res) * val); break;
        case DIV: resultSetInt(res,resultGetInt(res) / val); break;
        case NEQ:
          resultSetInt(res,resultGetInt(res) && prev != val); prev = val;
          break; // todo - way to finish early
        case EQ:
          resultSetInt(res,resultGetInt(res) && prev == val); prev = val;
          break;
        case GT: 
          resultSetInt(res,resultGetInt(res) && prev > val); prev = val;
          break;
        case LT:
          resultSetInt(res,resultGetInt(res) && prev < val); prev = val;
          break;
        default: break;
        }


      node = CDR(node);
    }
  }else{
    enum TokenType type = node->atom.type;
    if (type == IDENTIFIER){
      struct Binding* var = deref(g_env,node->atom.name);
      resultSetInt(res,var->val);
    }else{
      resultSetInt(res,node->atom.val); 
    }

  }
  return 0;
}




int evalIF(struct Node* if_node, struct Result* res){
  struct Node* node = CDR(if_node);
  struct Result pred_val;                      
  if (evalSExpression(CAR(node), &pred_val) == 1) return 1;

  struct Node* branch = CDR(node);
  struct Node* seq = CAR(branch); 
  struct Node* alt = CAR(CDR(branch)); // todo - error check
  
  if (resultGetInt(&pred_val) != 0){
    return evalSExpression(seq,res);

  }else{
    return  evalSExpression(alt,res);
  }
  
}

int evalSwitch(struct Node* switch_node, struct Result* res){
  struct Node* node = CDR(switch_node);
  struct Result switch_val;
  evalSExpression(CAR(node),&switch_val);
  node = CDR(node);
  
  while (node != NULL){ // todo - or default case
    struct Node* cur_case = CAR(CDR(CAR(node))); // as first child will be keyword case
    struct Node* case_expr = CAR(CDR(CDR(CAR(node))));

    struct Result case_val;
    evalSExpression(cur_case,&case_val);
    
    if (resultGetInt(&case_val) == resultGetInt(&switch_val)){ //todo - resultCompInt
      return evalSExpression(case_expr, res);
    }
    node= CDR(node);
  }
  return 1;
}



int evalWhile(struct Node* while_node, struct Result* res){
  struct Node* node = CDR(while_node);
  struct Result pred_val;                      
  if (evalSExpression(CAR(node), &pred_val) == 1) return 1;

  struct Node* init_seq =  CDR(node);

  while (resultGetInt(&pred_val) != 0){
    struct Node* cur_seq  = init_seq;
    
    evalSeq(cur_seq,res);
    
    if (evalSExpression(CAR(node), &pred_val) == 1) return 1; //todo - need for all evals create macro 
  }
  return 0;
}



int evalAssign(struct Node* assign_node, struct Result* res){
  struct Node* node = CDR(assign_node);
  char* name = CAR(node)->atom.name; //todo - check if identifier type // todo - macro NAME
  
  struct Result value;
  evalSExpression(CAR(CDR(node)),&value);

  struct Binding* var = deref(g_env,name);
  if (var == NULL){
    assign(g_env,name,resultGetInt(&value));

  }else{
    var->val = resultGetInt(&value);
  }
  resultSetInt(res,resultGetInt(&value));
  return 0;

}



int evalLet(struct Node* let_node, struct Result* res){
  struct Node* node = CDR(let_node);
  struct Node* param_list = CAR(node);
  struct Node* cur_param = param_list;
  g_env = enterEnv(g_env);  // todo - modify it when pass it in

  while (cur_param != NULL){
    struct Node* id_node = CAR(CAR(cur_param));
    struct Node* val_node = CAR(CDR(CAR(cur_param)));
    struct Result val;
    evalSExpression(val_node,&val);
    char* name = id_node->atom.name;
    assign(g_env,name,resultGetInt(&val));

    cur_param = CDR(cur_param);
  }

  resultSetInt(res,0);
  evalSeq(CDR(node),res);

  g_env = leaveEnv(g_env);

  return 0;
}


int evalSeq(struct Node* seq, struct Result* res){
  while (seq != NULL){
    evalSExpression(CAR(seq),res);
    seq = CDR(seq);
  }
  return 0;
}


int evalCallLambda(struct Node* eval_lambda_node, struct Result* res){
  struct Node* node = CDR(eval_lambda_node);
  struct Node* lambda = CAR(node);
  struct Node* args = CDR(node);
  struct Node* params = CAR(CDR(lambda));
  struct Node* body = CDR(CDR(lambda));
  g_env = enterEnv(g_env);

  while (params != NULL){ // todo - need way to check if there is an args
    char* name = CAR(params)->atom.name;
    struct Result val_result;
    evalSExpression(CAR(args),&val_result);
    params = CDR(params);
    args = CDR(args);
    assign(g_env,name,resultGetInt(&val_result));
  }
  evalSeq(body,res);
  g_env = leaveEnv(g_env);
  
  return 0;
}

 int evalDefFunction(struct Node* seq, struct Result* res){   

   return 0;
 }





void resultSetInt(struct Result* res, int value){
  res->type = INT_TYPE;
  res->int_val = value;
}
int resultGetInt(struct Result* res){
  //todo - add type check;
  return  res->int_val;
}

void resultSetList(struct Result* res, struct Node* list){
  res->type = LIST_TYPE;
  res->list_val = list;
}

struct Node* resultGetList(struct Result* res){
  return  res->list_val;
}


