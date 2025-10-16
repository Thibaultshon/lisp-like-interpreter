#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>   
#include "tokenize.h"  

struct Parser{
  int curPos;
  struct Token lookAhead;
  bool hasLookAhead;
  int lookAheadPos;
};


struct Node {
  bool is_atom;

  union{
    struct Token atom;

    struct{
      struct Node* cdr;
      struct Node* car;
    }cons;
  };
};


struct Node* parse(struct Parser* parser, char* input);
void freeParser(struct Parser* parser);
void initParser(struct Parser* parser);
struct Token peek(struct Parser* parser, char* input);
void freeNode(struct Node* node);
void printNode(struct Node* node);


#endif
