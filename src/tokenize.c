#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  
#include <stdbool.h>
#include <string.h> 

#include "util.h"
#include "tokenize.h"

#define MAXTOKENS 100





struct Item{
  char* name;
  enum TokenType token;
};



struct Token inKeyWords (char* string){ // todo - make into proper hashtable
  struct Item items[]   = { {"if", IF}, {"while",  WHILE}, {"let", LET},{"switch", SWITCH}, {"case", CASE}};  // todo - move assign to case
  size_t length  = sizeof(items)/sizeof(items[0]); 
  struct Token new_token;
  for (int i =0; i < length;i++){
    if  (strcmp(items[i].name,string)==0){
      new_token.type = items[i].token;
      return new_token;
    }
  };
  new_token.type = INVALID;
  return new_token;
}

struct Token createOpToken(enum TokenType type){
  struct Token token;
  token.type = type;
  return token;
}
struct Token createIntToken(int val){
  struct Token token;
  token.type = NUM;
  token.val = val;
  return token;
}

struct Token createIdentifierToken(char* string){
  struct Token token;
  token.type = IDENTIFIER;
  token.name = strdup(string);
  return token;
}


void freeTokens(struct Token *tokens){
  if (tokens == NULL) return;
  for (int i =0; tokens[i].type != END_LINE; i++){
    if (tokens[i].type == IDENTIFIER){
      free(tokens[i].name);
    }
  }
  free(tokens);
}



void printStringToTokens(char* input){
  const size_t maxsize= strlen(input);
  struct Token* tokens = malloc(maxsize * sizeof (struct Token));
  int i = 0;
  int index = 0;
  while ((tokens[index] = tokenize(input,&i)).type != END_LINE){
    printf("%s ",enumToString(tokens[index].type));
    index++;
  }
  tokens[index].type = END_LINE;
  freeTokens(tokens);
}



  
const char* enumToString(enum TokenType type) {
  switch(type){
  case NUM:
    return "num";
  case STRING:
    return "string";
  case IDENTIFIER:
    return "identifier";
  case ASSIGN:
    return "assign";
  case BRACK_OPEN:
    return "(";
  case BRACK_CLOSE:
    return ")";
  case EQ:
    return "=";
  case NEQ:
    return "/=";
  case GT:
    return ">";
  case LT:
    return "<";
  case ADD:
    return "+";
  case SUB:
    return "-";
  case DIV:
    return "/";
  case MUL:
    return "*";
  case END_LINE:
    return "end line";
  case IF:
    return "if";
  case WHILE:
    return "while";
  case SWITCH:
    return "switch";
  case CASE:
    return "CASE";
  case INVALID:
    return "INVALID";
  case LET:
    return "let";
  default:
    return "ERROR";

  }
}




struct Token tokenize  (char* input, int* i){
  while (isspace(input[*i])) (*i)++;
  if (input[*i]== '\0') return createOpToken(END_LINE);

  char c =  input[*i];
  (*i)++;
  
  switch (c){
  case '+':
    return createOpToken(ADD);
  case '-':
    return createOpToken(SUB);
  case '/':
    if (input[*i] == '='){
      c = input[(*i)++];
      return  createOpToken(NEQ);
    }
    return createOpToken(DIV);
  case '*':
    return createOpToken(MUL);
  case '>':
    return createOpToken(GT);
    break;
  case '<':
    return createOpToken(LT);
  case '(':
    return createOpToken(BRACK_OPEN);
  case ')':
    return createOpToken(BRACK_CLOSE);
  case ':':
    if (input[*i] == '='){
      c = input[(*i)++];
      return  createOpToken(ASSIGN);
    }
    return createOpToken(INVALID);
  case '=' :
    return createOpToken(EQ);
  default:
    if (isdigit(c)){ 
      char *end;
      int val = strtol(&input[(*i)-1],&end,10);
      
      *i = end - input;       
      return createIntToken(val);
    }
      
    else if (isalpha(c)){ /// is laphanum
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
        if (token.type == INVALID){
          token  = createIdentifierToken(result);
        }
        free(result);
        return token;

      }else{
      
      return createOpToken(INVALID);
      
    }
  }
}


