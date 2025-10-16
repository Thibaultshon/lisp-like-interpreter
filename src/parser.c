#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  
#include <string.h> 
#include "util.h"

#include "parser.h"




struct Token eat(struct Parser* parser, char* input);
struct Node* createCons(struct Node* nodecar, struct  Node* nodecdr);
struct Node* parseAtom(struct Parser* parser, char*input);
struct Node* parseSExpression(struct Parser* parser, char* input);

struct Token eat(struct Parser* parser, char* input){
  if (parser->hasLookAhead){
    struct Token  curToken = parser->lookAhead;
    parser->hasLookAhead = false;
    parser->curPos = parser->lookAheadPos;
    parser->lookAhead.type = END_LINE; 
    return curToken;
  }
  return tokenize(input, &parser->curPos);
}

struct Token peek(struct Parser* parser, char* input){
  if (parser->hasLookAhead){
    return parser->lookAhead;
  }
  if (parser->lookAhead.type == IDENTIFIER){ // free old tokens not eaten
    free(parser->lookAhead.name);
    parser->lookAhead.name = NULL;

  }
  int cur_pos = parser->curPos;   // create new token and add to lookahead
  struct Token curToken = tokenize(input,&cur_pos);
  parser->hasLookAhead = true;
  parser->lookAheadPos = cur_pos;
  parser->lookAhead =curToken;
  return curToken;
}


void freeParser(struct Parser* parser){
  if (parser->hasLookAhead && parser->lookAhead.type == IDENTIFIER){
    if (parser->lookAhead.name != NULL){
      free(parser->lookAhead.name);
    }
  }
}

void initParser(struct Parser* parser){
  parser->curPos = 0;
  parser->lookAhead.type = END_LINE;
  parser->lookAhead.name = NULL;
  parser->hasLookAhead = false;
  parser->lookAheadPos = 0;
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

