#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  
#include <string.h> 
#include "util.h"

#include "parser.h"



struct Token before(struct Parser* parser,char* input);
struct Token eat(struct Parser* parser, char* input);
struct Node* createCons(struct Node* nodecar, struct  Node* nodecdr);
struct Node* parseAtom(struct Parser* parser, char*input);
struct Node* parseSExpression(struct Parser* parser, char* input);

struct Token eat(struct Parser* parser, char* input){ 
  struct Token cur_token = tokenize(input, &parser->curPos);
  return cur_token;
}

struct Token peek(struct Parser* parser, char* input){
  int cur_pos = parser->curPos;
  return tokenize(input,&cur_pos); //todo  - maybe store prev and check if there so don't have to reread
}
struct Token before(struct Parser* parser, char* input){
  int prev_pos = parser->curPos -1;
  if (prev_pos < 0) prev_pos = 0;
  return tokenize(input,&prev_pos);
}


void freeNode(struct Node* node){
  if (!node){
    return ;
  }
  if(!node->is_atom){
    freeNode(node->cons.car);
    freeNode(node->cons.cdr);
  }else{
    if (node->atom.type == IDENTIFIER){
      free(node->atom.name);           
    }                                 
  }
  
  free(node);
}



void printNode(struct Node* node){
  if (!node){
    printf("nil");
    return ;
  }
  if(!node->is_atom){
    printf("(cons ");
    printNode(node->cons.car);
    printNode(node->cons.cdr);
    printf(")");
  }else{
    printf("%s ",enumToString(node->atom.type));
  }
  
}


struct Node* parseAtom(struct Parser* parser, char* input){
  struct Node* node = malloc(sizeof(struct Node));
  node->atom = eat(parser,input);
  node->is_atom = true;
  return node;
}

struct Node* createCons(struct Node* nodecar, struct  Node* nodecdr){
  struct Node* cur_cons = malloc(sizeof(struct Node));
  cur_cons->is_atom = false;
  CAR(cur_cons) = nodecar;
  CDR(cur_cons) = nodecdr;
  return cur_cons;

}



struct Node* parseSExpression(struct Parser* parser, char* input){
  if (peek(parser,input).type == BRACK_OPEN){
    eat(parser,input);

    struct Node* head;
    if (peek(parser,input).type != BRACK_CLOSE ){
      bool is_head = true;
      struct Node* prev_cons;
      while (peek(parser,input).type != BRACK_CLOSE){
        struct Node* cur_node = parseSExpression(parser,input);
        struct Node* cur_cons = createCons(cur_node,NULL);
        if (!is_head){
          CDR(prev_cons) = cur_cons;
        }else{
          head = cur_cons;
          is_head = false;
        }
        prev_cons = cur_cons;

      }
      eat(parser,input); //todo - consume ) // add in if statment to check if ) otherwise throw error

      return head;
    }
  }
  else{
    return parseAtom(parser, input); // todo - maybe change to create atom
  }
  return NULL; //todo - error
}




struct Node*  parse(struct Parser* parser, char* input){
  return parseSExpression(parser, input);
}

