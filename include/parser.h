#ifndef PARSER_H
#define PARSER_H

struct Parser{
  int curPos;
  struct Token* tokens;
  struct Node* ast;
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


struct Node*  parse(struct Parser* parser);
void freeNode(struct Node* node);
void printNode(struct Node* node);
struct Token peak(struct Parser* parser);

#endif
