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




void freeTokens(struct Token *tokens){
  if (tokens == NULL) return;
  for (int i =0; tokens[i].type != END_LINE; i++){
    if (tokens[i].type == IDENTIFIER){
      free(tokens[i].name);
    }
  }
  free(tokens);
}


struct Token inKeyWords (char* string){ // todo - make into proper hashtable
  struct Item items[]   = { {"if", IF}, {"while",  WHILE}, {"lambda", LAMBDA},{"switch", SWITCH}, {"case", CASE}};  // todo - move assign to case
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
  case LAMBDA:
    return "lambda";
  case IF:
    return "if";
  case WHILE:
    return "while";
  case SWITCH:
    return "switch";
  case CASE:
    return "CASE";
  default:
    return "ERROR";

  }
}




struct Token* lexer  (char* input){
  struct Token* tokens = malloc(MAXTOKENS* sizeof (struct Token));
  int cur_token = 0;
  int i = 0;
  char c;
  while ((c= input[i++]) != '\0'){ //or read input// or EOF
    /* printf("%c",c); */
    switch (c){
    case '+':
      tokens[cur_token++] = createOpToken(ADD);
      break;
    case '-':
      tokens[cur_token++] = createOpToken(SUB);
      break;
    case '/':
      tokens[cur_token++] = createOpToken(DIV);
      break;
    case '*':
      tokens[cur_token++] = createOpToken(MUL);
      break;
    case '(':
      tokens[cur_token++] = createOpToken(BRACK_OPEN);
      break; 
    case ')':
      tokens[cur_token++] = createOpToken(BRACK_CLOSE);
      break; 
    case ':':
      c = input[i++];
      if (c != '\0'){
          if (c == '='){
              tokens[cur_token++] =   createOpToken(ASSIGN);
            }
        }
      break;
    case ' ':
    case '\t':
    case '\n':
      break; 
    default:
      if (isdigit(c)){ 
        char *end;
        int val = strtol(&input[i-1],&end,10);
        tokens[cur_token++] = createIntToken(val);
        i = end - input;       
      }
      
      else { /// todo - change if doen't need new string each time - simplify
        char *start= &input[i-1];
        char* end = start;
        while (*end && isalnum(*end)){
          end++;          
        }
        size_t length = end - start;
        char *result = malloc(length + 1);
        memcpy(result,start,length);
        result[length] = '\0';

        struct Token token = inKeyWords(result);
        i = (int)(end - input);
        if (!(token.type == INVALID)){
          tokens[cur_token++] = token;

        }
        else{
          tokens[cur_token++] = createIdentifierToken(result);
          /* error("lex error"); */
        }
        free(result);
    }
    }
  }
  tokens[cur_token].type = END_LINE;
  return tokens;
}
