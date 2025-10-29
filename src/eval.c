#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  


#include <string.h> 
#include "util.h"
#include "tokenize.h"
#include "eval.h"


int evalSExpression(struct Node* node, struct Result* res);
int evalCons(struct Node* node, struct Result* res);
int evalSymbol(struct Node* node, struct Result* res);
int evalIF(struct Node* node, struct Result* res);
int evalSwitch(struct Node* node, struct Result* res);
int evalWhile(struct Node* node, struct Result* res);
int evalAssign(struct Node* node, struct Result* res);
int evalLet(struct Node* node, struct Result* res);
int evalCallLambda(struct Node* lambda_node,struct Node* args_node, struct Result* res);
int evalLambda(struct Node* lambda_node, struct Result* res);
int evalSeq(struct Node* seq, struct Result* res);
int evalDefFunction(struct Node* seq, struct Result* res);


void resultSetInt(struct Result* res, int value);
struct Result* createIntResult(int num);
struct Result* createLambdaResult(struct Node* lambda);
void resultSetLambda(struct Result* res, struct Node* list);
void resultSetError(struct Result* res,char* error);
void resultSetSymbol(struct Result* res,char* symbol);


struct EnvFrame* g_env = NULL;


// todo - report line error was on
// todo - error stack list of errors as goes up 
#define TRY(stmt)  \
  if ((stmt)!= 0) return 1;


/* todo - snprintf or vsnprintf for more specific reporting*/
#define ERROR(res,error)                          \
  do{                                             \
    resultSetError((res),(error));                \
    return 1;                                     \
  }while(0)                                       





int eval(struct Node* node,struct Result* res){
  return evalSExpression(node,res);
}


int evalSExpression(struct Node* node, struct Result* res){
  if (node == NULL){
    resultSetInt(res,0);
    return 0;
  } 
  switch (node->type)
    {
    case NODE_CONS:
      return evalCons(node,res);
    case NODE_NUM:
      resultSetInt(res,node->atom.num);
      break;
    case NODE_SYMBOL:
      return evalSymbol(node,res);
    case NODE_KEYWORD:
      resultSetSymbol(res,node->atom.symbol);
      break;
    }
  return 0;
}


int evalSymbol(struct Node* node, struct Result* res){
  struct Binding* var = deref(g_env,node->atom.symbol);
  switch (var->res->type)
    {
    case RESULT_INT:
      resultSetInt(res,resultGetInt(var->res));
      break;
    case RESULT_LAMBDA:
      resultSetLambda(res,resultGetLambda(var->res));
      break;
    default:
      ERROR(res,"ERROR: variable value not of correct type");
    }
  return 0;
}


int evalCons(struct Node* node, struct Result* res){
  int prev;
  char* symbol;
  struct Result car_result;
  TRY(evalSExpression(CAR(node),&car_result));
  switch (car_result.type)
    {
    case RESULT_LAMBDA:
      {
      struct Node* args = CDR(node);
      return evalCallLambda(resultGetLambda(&car_result), args,res);
      }
    case RESULT_SYMBOL:
      symbol = resultGetSymbol(&car_result);
      break;
    default:
      ERROR(res,"ERROR: missing function at head");
    }

  if (strcmp(symbol, "if") == 0){
    return evalIF(node, res); 
  }
  if (strcmp(symbol, "while") ==0){
    return evalWhile(node, res);
  }
  if (strcmp(symbol, "switch") ==0){
    return evalSwitch(node,res);
  }
  if (strcmp(symbol, ":=") == 0){
    return evalAssign(node,res);
  }
  if (strcmp(symbol, "let") == 0){
    return evalLet(node,res);
  }if (strcmp(symbol, "lambda") == 0){
    return evalLambda(node,res);
  }if (strcmp(symbol, "def") == 0){
    return evalDefFunction(node,res);
  }

  if (strcmp(symbol, "+") == 0){
    resultSetInt(res,0);
  }else if (strcmp(symbol, "/=") ==0 || strcmp(symbol, "=") ==0|| strcmp(symbol, ">") ==0 || strcmp(symbol, "<")==0){
    node = CDR(node);
    struct Result prev_res;
    TRY(evalSExpression(CAR(node), &prev_res));
    prev = resultGetInt(&prev_res);
    resultSetInt(res,1);
    
  }else if (strcmp(symbol, "*") ==0){ 
    resultSetInt(res,1);
      
  } else if (strcmp(symbol, "/") ==0|| strcmp(symbol, "-") ==0) {
    node = CDR(node);
    TRY(evalSExpression(CAR(node), res));
    
  } else{
    ERROR(res,"ERROR: no keyword with that name");
  }

  node = CDR(node);

  while(node){
    struct Result val_result;
    evalSExpression(CAR(node), &val_result);
    int val = resultGetInt(&val_result);
    if (strcmp(symbol, "+") ==0){
      resultSetInt(res,resultGetInt(res) + val);
    } else if (strcmp(symbol, "-") ==0){                        
      resultSetInt(res,resultGetInt(res) - val); 
    } else if (strcmp(symbol, "*") ==0){  
      resultSetInt(res,resultGetInt(res) *val); 
    }else if (strcmp(symbol, "/") ==0){  
      resultSetInt(res,resultGetInt(res) / val); 
    }else if (strcmp(symbol, "/=") ==0){
      resultSetInt(res,resultGetInt(res) && prev != val); prev = val;
    }else if (strcmp(symbol, "=") ==0){
      resultSetInt(res,resultGetInt(res) && prev ==  val); prev = val;
    }else if (strcmp(symbol, ">") ==0){
      resultSetInt(res,resultGetInt(res) && prev >  val); prev = val;
    }else if (strcmp(symbol, "<") ==0){
      resultSetInt(res,resultGetInt(res) && prev <  val); prev = val;
    }
    node = CDR(node);
  }
  return 0;
}


int evalIF(struct Node* if_node, struct Result* res){
  struct Node* node = CDR(if_node);
  struct Result pred_val;                      
  TRY(evalSExpression(CAR(node), &pred_val));

  struct Node* branch = CDR(node);
  struct Node* seq = CAR(branch);   //todo - if only seq provided
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
  TRY(evalSExpression(CAR(node),&switch_val));
  node = CDR(node);
  
  while (node != NULL){ // todo - or default case
    struct Node* cur_case = CAR(CDR(CAR(node))); // as first child will be keyword case
    struct Node* case_expr = CAR(CDR(CDR(CAR(node))));

    struct Result case_val;
    TRY(evalSExpression(cur_case,&case_val));
    
    if (resultGetInt(&case_val) == resultGetInt(&switch_val)){ //todo - resultCompInt
      return evalSExpression(case_expr, res);
    }
    node= CDR(node);
  }

  ERROR(res,"ERROR: no case matching predicate");
}



int evalWhile(struct Node* while_node, struct Result* res){
  struct Node* node = CDR(while_node);
  struct Result pred_val;                      
  TRY(evalSExpression(CAR(node), &pred_val));

  struct Node* init_seq =  CDR(node);

  while (resultGetInt(&pred_val) != 0){
    struct Node* cur_seq  = init_seq;
    TRY(evalSeq(cur_seq,res));
    TRY(evalSExpression(CAR(node), &pred_val));
  }
  return 0;
}


int evalAssign(struct Node* assign_node, struct Result* res){
  struct Node* node = CDR(assign_node);

  if (CAR(node)->type != NODE_SYMBOL){
    ERROR(res,"ERROR: id not a symbol");
  }
  char* name =  CAR(node)->atom.symbol;
  
  struct Result* value = createIntResult(0); //todo - check if value of lambda or int 
  TRY(evalSExpression(CAR(CDR(node)),value));

  struct Binding* var = deref(g_env,name);
  resultSetInt(res,resultGetInt(value));
  if (var == NULL){
    assign(g_env,name,value);
  }else{
    resultSetInt(var->res,resultGetInt(value));
    free(value);
  }
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

    struct Result* val = createIntResult(0);
    TRY(evalSExpression(val_node,val));
    char* name = id_node->atom.symbol;
    declare(g_env,name,val);

    cur_param = CDR(cur_param);
  }

  resultSetInt(res,0);
  TRY(evalSeq(CDR(node),res));

  g_env = leaveEnv(g_env);

  return 0;
}


int evalSeq(struct Node* seq, struct Result* res){
  while (seq != NULL){
    TRY(evalSExpression(CAR(seq),res));
    seq = CDR(seq);
  }
  return 0;
}


int evalCallLambda(struct Node* lambda_node, struct Node* args_node, struct Result* res){
  struct Node* args = args_node;
  struct Node* params = CAR(lambda_node);
  struct Node* body = CDR(lambda_node); 
  g_env = enterEnv(g_env);


  while (params != NULL){ // todo - need way to check if there is an args
    char* name = CAR(params)->atom.symbol;

    struct Result* val_result = createIntResult(0);

    TRY(evalSExpression(CAR(args),val_result));
    declare(g_env,name,val_result);
    params = CDR(params);
    args = CDR(args);
  }

  resultSetInt(res,0); // incase no body
  TRY(evalSeq(body,res));
  g_env = leaveEnv(g_env);
  
  return 0;
}

int evalLambda(struct Node* lambda_node, struct Result* res){
  resultSetLambda(res,CDR(lambda_node));
  // cdr to skip lambda part so that funcs and lambda can be called in same way
  return 0;
 }


int evalDefFunction(struct Node* def_node, struct Result* res){
  struct Node* node = CDR(def_node);
  char * name = CAR(node)->atom.symbol;
  struct Node * lambda = CDR(CDR(def_node));
  struct Result* lambda_node = createLambdaResult(lambda);
  assign(g_env, name, lambda_node);
  resultSetSymbol(res,name);
  return 0;
}


void resultSetInt(struct Result* res, int value){
  res->type = RESULT_INT;
  res->int_val = value;
}


int resultGetInt(struct Result* res){
  //todo - add type check;
  return  res->int_val;
}

struct Result* createIntResult(int num){
  struct Result* int_res = malloc(sizeof(*int_res));
  resultSetInt(int_res,num);
  return int_res;
}



struct Result* createLambdaResult(struct Node* lambda){
  struct Result* lambda_res = malloc(sizeof(*lambda_res));
  resultSetLambda(lambda_res,lambda);
  return lambda_res;
}


void resultSetLambda(struct Result* res, struct Node* list){
  res->type = RESULT_LAMBDA;
  res->list_val = list;
}

struct Node* resultGetLambda(struct Result* res){
  return  res->list_val;
}

void resultSetSymbol(struct Result* res,char* symbol){
  res->type = RESULT_SYMBOL;
  res->symbol_val = symbol;
}


char* resultGetSymbol(struct Result* res){
  return  res->symbol_val;
}

void resultSetError(struct Result* res,char* error){
  res->type = RESULT_SYMBOL;
  res->symbol_val = error;
}

char* resultGetError(struct Result* res){
  return  res->symbol_val;
}

void printResult(struct Result* result){
  switch(result->type)
    {
    case RESULT_SYMBOL:
    case RESULT_ERROR:
    case RESULT_STRING:
      printf("%s",result->symbol_val);
      break;
    case RESULT_INT:
      printf("%d",result->int_val);
      break;
    case RESULT_LAMBDA:
      printf("(lambda)");
      /* printNodes(lambda); */
      /* break; */
    }
}
