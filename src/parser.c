#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  
#include <stdbool.h>
#include <string.h> 

#include "util.h"
#include "tokenize.h"
#include "parser.h"



struct Token before(struct Parser* parser);
struct Token eat(struct Parser* parser);
struct Token peak(struct Parser* parser);
struct Node* createCons(struct Node* nodecar, struct  Node* nodecdr);
struct Node* parseAtom(struct Parser* parser);
struct Node* parseSExpression(struct Parser* parser);

struct Token eat(struct Parser* parser){    //fix this check if at end before eating
  struct Token cur_token = parser->tokens[parser->curPos];
  if (cur_token.type != END_LINE){
    parser->curPos++;
  }
  return cur_token;
}



struct Token peak(struct Parser* parser){
  return parser->tokens[parser->curPos]; 
}
struct Token before(struct Parser* parser){
  return parser->tokens[parser->curPos-1]; 
}


void freeNode(struct Node* node){
  if (!node){
    return ;
  }
  if(!node->is_atom){
    freeNode(node->cons.car);
    freeNode(node->cons.cdr);
  }
  free(node);
}



void printNode(struct Node* node){
  if (!node){
    return ;
  }
  if(!node->is_atom){
    printf("cons ");
    printNode(node->cons.car);
    printNode(node->cons.cdr);
  }else{
    printf("%s ",enumToString(node->atom.type));
  }
  
}


struct Node* parseAtom(struct Parser* parser){
  struct Node* node = malloc(sizeof(struct Node));
  node->atom = eat(parser);
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



struct Node* parseSExpression(struct Parser* parser){
  if (peak(parser).type == BRACK_OPEN){
    eat(parser);

    struct Node* head;
    if (peak(parser).type != BRACK_CLOSE ){
      bool is_head = true;
      struct Node* prev_cons;
      while (peak(parser).type != BRACK_CLOSE){
        struct Node* cur_node = parseSExpression(parser);
        struct Node* cur_cons = createCons(cur_node,NULL);
        if (!is_head){
          CDR(prev_cons) = cur_cons;
        }else{
          head = cur_cons;
          is_head = false;
        }
        prev_cons = cur_cons;

      }
      eat(parser); //consume ) // add in if statment to check if ) otherwise throw error

      return head;
    }
  }
  else{
    return parseAtom(parser); // maybe change to create atom
  }
  return NULL; //error
}




struct Node*  parse(struct Parser* parser){
  return parseSExpression(parser);
}

