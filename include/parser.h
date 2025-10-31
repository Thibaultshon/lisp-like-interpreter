#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>   
#include "tokenize.h"  

struct Parser{
  int cur_pos;
  struct Token look_ahead; // todo - struct look ahead
  bool has_look_ahead;
  int look_ahead_pos;
};

enum ParseResultType{
  PARSE_RESULT_SUCCESS,
  PARSE_RESULT_MALFORMED_CONS,
  PARSE_RESULT_WRONG_LEXEME
};


enum NodeType{
  NODE_LAMBDA,
  NODE_CONS,
  
  // atoms
  NODE_NUM, 
  NODE_KEYWORD, //todo - later make just a symbol
  NODE_STRING,
  NODE_SYMBOL
};

////* AST *////
struct Node {
  enum NodeType type;
  union{
    struct {
      union {
        int num;
        char* symbol;
      };
    }atom;

    struct{
      struct Node* cdr;
      struct Node* car;
    }cons;
  };
};

#define CAR(node) ((node)->cons.car) //to use for assinging
#define CDR(node) ((node)->cons.cdr)

static inline struct Node* car(struct Node* node){
  if (node && node->type == NODE_CONS){
    return node->cons.car;
  }else{
    return NULL;
  }
}

static inline struct Node* cdr(struct Node* node){
  if (node && node->type == NODE_CONS){
    return node->cons.cdr;
  }else{
    return NULL;
  }
}


static inline bool nodeContainsString(struct Node* node){
  if (!node) return false;
   return (node->type == NODE_STRING ||
          node->type == NODE_SYMBOL ||
          node->type == NODE_KEYWORD) &&
     (node->atom.symbol != NULL);
}

static inline bool isAtom(struct Node* node){
  if (!node) return false;
  return  (node->type == NODE_KEYWORD ||
           node->type == NODE_SYMBOL ||
           node->type == NODE_KEYWORD ||
           node->type == NODE_STRING);
}


enum ParseResultType parse(struct Parser* parser, char* input,struct Node** node);
void freeParser(struct Parser* parser);
void initParser(struct Parser* parser);
struct Token peek(struct Parser* parser, char* input);
void freeNode(struct Node* node);
void printNode(struct Node* node);

const char* parseTypeToString(enum ParseResultType type);
#endif
