#include <stdio.h>  
#include <stdlib.h> 
#include <ctype.h>  
#include <string.h> 
#include <stdio.h> 

#include "util.h"
#include "parser.h"


struct Token eat(struct Parser* parser, char* input);
struct Node* createCons(struct Node* nodecar, struct  Node* nodecdr);
enum ParseResultType parseAtom(struct Parser* parser, char*input, struct Node** node);
enum ParseResultType parseSExpression(struct Parser* parser, char* input,struct Node** node);
enum ParseResultType parseClosingBracket(struct Parser* parser, char*input);


void* readMalloc(size_t size){
  void* ptr = malloc(size);
  if (ptr == NULL){
    fprintf(stderr, "Not enough memory");
    exit(74);
  }
  return ptr;
}


enum ParseResultType parse(struct Parser* parser, char* input,struct Node** node){
  return parseSExpression(parser, input,node);
}


enum ParseResultType parseSExpression(struct Parser* parser, char* input,struct Node** head){
  if (peek(parser,input).type == TOK_BRACK_OPEN){
    free(eat(parser,input).name);

    if (peek(parser,input).type != TOK_BRACK_CLOSE ){

      bool is_head = true;
      struct Node* prev_cons= NULL;
      while (peek(parser,input).type != TOK_BRACK_CLOSE){        

        struct Node* cur_node = NULL;
        
        enum ParseResultType result=parseSExpression(parser,input,&cur_node);
        if (result != PARSE_RESULT_SUCCESS) return result;

        struct Node* cur_cons = createCons(cur_node,NULL);
        
        if (!is_head){
          CDR(prev_cons) = cur_cons;

        }else{

          *head = cur_cons;
          is_head = false;
        }
        prev_cons = cur_cons;
      }

      return parseClosingBracket(parser,input);

    }else{
      *head = NULL;
      return parseClosingBracket(parser,input);

    }
  }
  else{
    enum ParseResultType result=parseAtom(parser, input,head);
    if (result != PARSE_RESULT_SUCCESS) return result;
  }
}

enum ParseResultType parseClosingBracket(struct Parser* parser, char*input){
  struct Token tok = eat(parser,input);
  if (tok.type  == TOK_BRACK_CLOSE){
    free(tok.name);
    return  PARSE_RESULT_SUCCESS;
  }else{
    free(tok.name);
    return PARSE_RESULT_MALFORMED_CONS;
  }
}



enum ParseResultType parseAtom(struct Parser* parser, char* input,struct Node** node){
  *node = readMalloc(sizeof(**node));

  struct  Token cur_tok = eat(parser,input);
  switch (cur_tok.type){
  case TOK_NUM:
    (*node)->type = NODE_NUM;
    (*node)->atom.num = cur_tok.num;
    break;
  case TOK_STRING:
    (*node)->type = NODE_STRING;
    (*node)->atom.symbol  = cur_tok.name;
    break;
  case TOK_OP:
    (*node)->type = NODE_KEYWORD;
    (*node)->atom.symbol  = cur_tok.name;
    break;
  case TOK_IDENTIFIER:
    (*node)->type = NODE_SYMBOL;
    (*node)->atom.symbol  =cur_tok.name;
    break;
  case TOK_INVALID:
    free(*node);
    *node = NULL;
    return  PARSE_RESULT_WRONG_LEXEME;
  default:
    if (cur_tok.name) free(cur_tok.name);
    free(*node);
    *node = NULL;
    return PARSE_RESULT_MALFORMED_CONS;
  }
  return   PARSE_RESULT_SUCCESS;    
}


struct Node* createCons(struct Node* node_car, struct  Node* node_cdr){
  struct Node* cur_cons = readMalloc(sizeof(*cur_cons));
  cur_cons->type = NODE_CONS;
  CAR(cur_cons) = node_car;
  CDR(cur_cons) = node_cdr;
  return cur_cons;

}


struct Token eat(struct Parser* parser, char* input){
  if (parser->has_look_ahead){
    struct Token  cur_token = parser->look_ahead;
    parser->has_look_ahead = false;
    parser->cur_pos = parser->look_ahead_pos;
    parser->look_ahead.type = TOK_END_LINE;
    parser->look_ahead.name = NULL;
    return cur_token;

  }
  return tokenize(input, &parser->cur_pos);
}


struct Token peek(struct Parser* parser, char* input){
  if (parser->has_look_ahead){
    return parser->look_ahead;
  }
  int cur_pos = parser->cur_pos;   // create new token and add to lookahead
  struct Token curToken = tokenize(input,&cur_pos);
  parser->has_look_ahead = true;
  parser->look_ahead_pos = cur_pos;
  parser->look_ahead =curToken;
  return curToken;
}


void freeParser(struct Parser* parser){
  if (parser == NULL) return;
  if (parser->has_look_ahead && tokenContainsString(&parser->look_ahead)){
    if (parser->look_ahead.name != NULL){      
      free(parser->look_ahead.name);
    }
  }
}


void initParser(struct Parser* parser){
  parser->cur_pos = 0;
  parser->look_ahead.type = TOK_END_LINE;
  parser->look_ahead.name = NULL;
  parser->has_look_ahead = false;
  parser->look_ahead_pos = 0;
}


void freeNode(struct Node* node){
  if (!node){
    return ;
  }
  if(node->type == NODE_CONS){
    freeNode(node->cons.car);
    freeNode(node->cons.cdr);
  }else{
    if (nodeContainsString(node)){
      free(node->atom.symbol);
      node->atom.symbol = NULL;
    }
  }
  free(node);
}


void printNode(struct Node* node){
  if (!node){
    return ;
  }
  switch(node->type)
    {
    case NODE_CONS:
      printf("(cons ");
      printNode(node->cons.car);
      printNode(node->cons.cdr);
      printf(")");
      break;
    case NODE_NUM:
      printf("%d ", node->atom.num);
      break;
    default:
      if (nodeContainsString(node)){
        printf("%s ", node->atom.symbol);
      }
    }

}

const char* parseTypeToString(enum ParseResultType type) { 
  switch(type)
    {
    case PARSE_RESULT_SUCCESS:
      return "Success";
    case PARSE_RESULT_MALFORMED_CONS:
      return "Parsing Error: malformed list";
    case PARSE_RESULT_WRONG_LEXEME:
      return "Lexical Error: not valid lexeme";
    default:
      return "PARSE ERROR";
    }
}

