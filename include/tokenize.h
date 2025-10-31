#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdbool.h>   

enum TokenType {
  TOK_NUM,
  TOK_STRING,
  TOK_BRACK_OPEN,
  TOK_BRACK_CLOSE,
  TOK_OP,
  TOK_IDENTIFIER,
  TOK_INVALID,
  TOK_END_LINE
};


struct Token {
  enum TokenType type;
  union {
    int num;
    char* name;
  };
};


static inline bool tokenContainsString(struct Token* tok){
  if (!tok)  return false;
  return (tok->type == TOK_STRING ||
          tok->type == TOK_OP ||
          tok->type == TOK_BRACK_OPEN ||
          tok->type == TOK_BRACK_CLOSE ||
          tok->type == TOK_IDENTIFIER) &&
    (tok->name != NULL);
}


void printStringToTokens(char* input);

const char* enumToString(enum TokenType type);

struct Token tokenize  (char* input, int* i);

#endif

