#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  
#include <stdbool.h>
#include <string.h> 


#include "util.h"
#include "tokenize.h"


#define MAXTOKENS 100


struct Token inKeyWords (char* string);
struct Token createToken(enum TokenType type,char* symbol);
struct Token createIntToken(int num);
struct Token createIdentifierToken(char* string);
void freeTokens(struct Token *tokens);
void printStringToTokens(char* input);
const char* enumToString(enum TokenType type);
struct Token tokenize  (char* input, int* i);


struct Item{
  char* name;
  enum TokenType token;
};


struct Token tokenize  (char* input, int* i){
  while (isspace(input[*i])) (*i)++;
  if (input[*i]== '\0') return createToken(TOK_END_LINE,NULL);

  char c =  input[*i];
  (*i)++;

  
  switch (c){
  case '+':
    return createToken(TOK_OP,"+");
  case '-':
    return createToken(TOK_OP,"-");
  case '/':
    if (input[*i] == '='){
      c = input[(*i)++];
      return  createToken(TOK_OP,"/=");
    }
    return createToken(TOK_OP,"/");
  case '*':
    return createToken(TOK_OP,"*");
  case '>':
    return createToken(TOK_OP,">");
    break;
  case '<':
    return createToken(TOK_OP,"<");
  case '(':
    return createToken(TOK_BRACK_OPEN,"(");
  case ')':
    return createToken(TOK_BRACK_CLOSE,")");
  case ':':
    if (input[*i] == '='){
      c = input[(*i)++];
      return  createToken(TOK_OP, ":=");
    }
    return createToken(TOK_INVALID,NULL);
  case '=' :
    return createToken(TOK_OP,"=");
  default:
    if (isdigit(c)){ 
      char *end;
      int val = strtol(&input[(*i)-1],&end,10);
      
      *i = end - input;       
      return createIntToken(val);
    }
      
    else if (isalpha(c)){ 
        struct Token token;
        char *start= &input[(*i)-1];
        char* end = start;
        while (*end && isalnum(*end)){
          end++;          
        }
        size_t length = end - start;
        char *result = malloc(length + 1);
        memcpy(result,start,length);
        result[length] = '\0';

        *i = (int)(end - input);

        token = inKeyWords(result);
        if (token.type == TOK_INVALID){
          token  = createIdentifierToken(result);
        }
        free(result);
        return token;

      }else{
      /* printf("Here"); */
      return createToken(TOK_INVALID,NULL);
      
    }
  }
}


struct Token inKeyWords (char* string){ // todo - make into proper hashtable and get rid of TOK_OP assume all TOK_OP
  struct Item items[]   = { {"if", TOK_OP}, {"while",  TOK_OP}, {"let", TOK_OP},{"lambda", TOK_OP},{"switch", TOK_OP}, {"case", TOK_OP}};  // todo - move assign to case
  size_t length  = sizeof(items)/sizeof(items[0]); 
  struct Token new_token;
  for (int i =0; i < length;i++){
    if  (strcmp(items[i].name,string)==0){
      return createToken(TOK_OP,items[i].name);
    }
  };
  new_token.type = TOK_INVALID;
  new_token.name = NULL;
  return new_token;
}


struct Token createToken(enum TokenType type,char* symbol){
  struct Token token;
  token.type = type;
  token.name = (symbol != NULL) ? strdup(symbol) : NULL;
  return token;
}


struct Token createIntToken(int num){
  struct Token token;
  token.type = TOK_NUM;
  token.num = num;
  return token;
}


struct Token createIdentifierToken(char* string){
  struct Token token;
  token.type = TOK_IDENTIFIER;
  token.name = strdup(string);
  return token;
}


void freeTokens(struct Token *tokens){
  if (tokens == NULL) return;
  for (int i =0; tokens[i].type != TOK_END_LINE; i++){
    if (tokenContainsString(&tokens[i])){
      free(tokens[i].name);
    }
  }
}


void printStringToTokens(char* input){
  const size_t max_size= strlen(input) +1;
  struct  Token* tokens = malloc(max_size *sizeof(*tokens));  
  int i = 0;
  int index = 0;
  while ((tokens[index] = tokenize(input,&i)).type != TOK_END_LINE){
    printf("%s ",enumToString(tokens[index].type));
    index++;
  }
  tokens[index].type = TOK_END_LINE;
  freeTokens(tokens);
  free(tokens);
}


const char* enumToString(enum TokenType type) {  //todo // maybe just print out name
  switch(type){
  case TOK_NUM:
    return "NUM";
  case TOK_STRING:
    return "STRING";
  case  TOK_BRACK_OPEN:
    return "(";
  case   TOK_BRACK_CLOSE:
    return ")";
  case   TOK_OP:
    return "OPERATION";
  case   TOK_IDENTIFIER:
    return "ID";
  case TOK_END_LINE:
    return "END LINE";
  case TOK_INVALID:
    return "INVALID";
  default:
    return "ERROR";

  }
}




