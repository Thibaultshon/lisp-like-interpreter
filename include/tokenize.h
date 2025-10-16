#ifndef TOKENIZE_H
#define TOKENIZE_H

enum TokenType {
  NUM,
  STRING,
  BRACK_OPEN,
  BRACK_CLOSE,
  ADD,
  SUB,
  DIV,
  MUL,

  GT,
  LT,
  NEQ,
  EQ,

  SWITCH,
  CASE,
  
  ASSIGN,
  LET,
  LAMBDA,

  IF,
  WHILE,

  IDENTIFIER,

  INVALID,
  END_LINE
};




struct Token {
  enum TokenType type;
  union {
    int val;
    char* name;
  };
};


void printStringToTokens(char* input);

const char* enumToString(enum TokenType type);

struct Token tokenize  (char* input, int* i);

#endif

