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


void resultSetInt(struct Result* res, int value);
struct Result* createIntResult(int num);
struct Result* createLambdaResult(struct Node* lambda);
void resultSetLambda(struct Result* res, struct Node* list);
void resultSetError(struct Result* res,char* error);
void resultSetSymbol(struct Result* res,char* symbol);


struct EnvFrame* g_env = NULL;


// todo - report line error was on
// todo - error stack list of errors as goes up 
#define TRY(stmt)                               \
  if ((stmt)!= 0) return 1;

#define TRY_AND_LEAVE_ENV(stmt)                               \
  if ((stmt)!= 0) do{g_env = leaveEnv(g_env); return 1 ;}while(0);


/* todo - snprintf or vsnprintf for more specific reporting*/
#define ERROR(res,error)                        \
  do{                                           \
    resultSetError((res),(error));              \
    return 1;                                   \
  }while(0)                                       
//todo - assert node

#define ERROR_AND_LEAVE_ENV(res,error)          \
  do{                                          \
  g_env = leaveEnv(g_env);                     \
  ERROR(res,error);                            \
  }while(0)                                   

#define ERROR_IF_NULL(res,stmt)                                 \
  if((stmt)==NULL)  ERROR((res),"ERROR: malformed statement");

#define ERROR_AND_LEAVE_ENV_IF_NULL(res,stmt)                           \
  if((stmt)==NULL)  ERROR_AND_LEAVE_ENV(res,"ERROR: malformed statement");

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
  if (var == NULL){
      ERROR(res,"ERROR: undefined variable");
      /* free(var->res); */
  }
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
  TRY(evalSExpression(car(node),&car_result));
  switch (car_result.type)
    {
    case RESULT_LAMBDA:
      {
        struct Node* args = cdr(node);
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
  }
  if (strcmp(symbol, "+") == 0){
    resultSetInt(res,0);
  }else if (strcmp(symbol, "/=") ==0 || strcmp(symbol, "=") ==0|| strcmp(symbol, ">") ==0 || strcmp(symbol, "<")==0){
    node = cdr(node);
    struct Result prev_res;
    TRY(evalSExpression(car(node), &prev_res));
    prev = resultGetInt(&prev_res);
    resultSetInt(res,1);
    
  }else if (strcmp(symbol, "*") ==0){ 
    resultSetInt(res,1);
      
  } else if (strcmp(symbol, "/") ==0|| strcmp(symbol, "-") ==0) {
    node = cdr(node);
    TRY(evalSExpression(car(node), res));
    
  } else{
    ERROR(res,"ERROR: no keyword with that name");
  }

  node = cdr(node);

  while(node){
    struct Result val_result;
    evalSExpression(car(node), &val_result);
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
    node = cdr(node);
  }
  return 0;
}


int evalIF(struct Node* if_node, struct Result* res){
  struct Node* node = cdr(if_node);
  struct Result pred_val;                      
  TRY(evalSExpression(car(node), &pred_val));

  struct Node* branch = cdr(node);
  struct Node* seq = car(branch);   //todo - if only seq provided
  struct Node* alt = car(cdr(branch)); // todo - error check
  ERROR_IF_NULL(res,branch);
  ERROR_IF_NULL(res,seq);
  ERROR_IF_NULL(res,alt);
  
  if (resultGetInt(&pred_val) != 0){
    return evalSExpression(seq,res);

  }else{
    return  evalSExpression(alt,res);
  }
  
}

int evalSwitch(struct Node* switch_node, struct Result* res){
  struct Node* node = cdr(switch_node);
  struct Result switch_val;
  TRY(evalSExpression(car(node),&switch_val));
  node = cdr(node);
  
  while (node != NULL){ // todo - or default case
    struct Node* cur_case = car(cdr(car(node))); // as first child will be keyword case
    struct Node* case_expr = car(cdr(cdr(car(node))));
    ERROR_IF_NULL(res,cur_case);
    ERROR_IF_NULL(res,case_expr);
    struct Result case_val;
    TRY(evalSExpression(cur_case,&case_val));
    
    if (resultGetInt(&case_val) == resultGetInt(&switch_val)){ //todo - resultCompInt
      return evalSExpression(case_expr, res);
    }
    node= cdr(node);
  }

  ERROR(res,"ERROR: no case matching predicate");
}



int evalWhile(struct Node* while_node, struct Result* res){
  struct Node* node = cdr(while_node);
  struct Result pred_val;                      
  TRY(evalSExpression(car(node), &pred_val));

  struct Node* init_seq =  cdr(node);

  while (resultGetInt(&pred_val) != 0){
    struct Node* cur_seq  = init_seq;
    TRY(evalSeq(cur_seq,res));
    TRY(evalSExpression(car(node), &pred_val));
  }
  return 0;
}


int evalAssign(struct Node* assign_node, struct Result* res){
  struct Node* node = cdr(assign_node);

  if (car(node)->type != NODE_SYMBOL){
    ERROR(res,"ERROR: id not a symbol");
  }
  char* name =  car(node)->atom.symbol;
  
  struct Result* value = createIntResult(0);
  TRY(evalSExpression(car(cdr(node)),value));

  switch(value->type)
    {
    case RESULT_LAMBDA:
      resultSetSymbol(res,name);
      break;
    case RESULT_INT:
      resultSetInt(res,resultGetInt(value));
      break;
    default:
      free(value);
      ERROR(res,"ERROR: unable to use value type in assignment");
    }
  assign(g_env, name, value);
  return 0;
}


int evalLet(struct Node* let_node, struct Result* res){
  struct Node* node = cdr(let_node);
  struct Node* param_list = car(node);
  struct Node* cur_param = param_list;
  g_env = enterEnv(g_env);  // todo - modify it when pass it in

  while (cur_param != NULL){
    struct Node* id_node = car(car(cur_param));
    struct Node* val_node = car(cdr(car(cur_param)));
    ERROR_AND_LEAVE_ENV_IF_NULL(res,id_node);
    ERROR_AND_LEAVE_ENV_IF_NULL(res,id_node);

    char* name = id_node->atom.symbol;
    
    struct Result* value = createIntResult(0);
    TRY_AND_LEAVE_ENV(evalSExpression(val_node,value));

    switch(value->type)
      {
      case RESULT_LAMBDA:
        resultSetSymbol(res,name);
        break;
      case RESULT_INT:
        resultSetInt(res,resultGetInt(value));
        break;
      default:
        free(value);
        ERROR_AND_LEAVE_ENV(res,"ERROR: unable to use value type in assignment");
      }

    declare(g_env,name,value);

    cur_param = cdr(cur_param);
  }

  resultSetInt(res,0);

  TRY_AND_LEAVE_ENV(evalSeq(cdr(node),res));

  g_env = leaveEnv(g_env);

  return 0;
}


int evalSeq(struct Node* seq, struct Result* res){
  while (seq != NULL){
    TRY(evalSExpression(car(seq),res));
    seq = cdr(seq);
  }
  return 0;
}


int evalCallLambda(struct Node* lambda_node, struct Node* args_node, struct Result* res){
  struct Node* args = args_node;
  struct Node* params = car(lambda_node);
  struct Node* body = cdr(lambda_node); 
  g_env = enterEnv(g_env);


  while (params != NULL){
    ERROR_AND_LEAVE_ENV_IF_NULL(res,args);
    char* name = car(params)->atom.symbol;

    struct Result* val_result = createIntResult(0);

    TRY_AND_LEAVE_ENV(evalSExpression(car(args),val_result));
    declare(g_env,name,val_result);
    params = cdr(params);
    args = cdr(args);
  }

  resultSetInt(res,0); // incase no body
  TRY_AND_LEAVE_ENV(evalSeq(body,res));
  g_env = leaveEnv(g_env);
  
  return 0;
}

int evalLambda(struct Node* lambda_node, struct Result* res){
  resultSetLambda(res,cdr(lambda_node));
  // cdr to skip lambda part so that funcs and lambda can be called in same way
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
  res->type = RESULT_ERROR;;
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
      /* printNode(result->list_val); */
      break;
    }
}
