#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>



#define MAXTOKENS 100


#define CAR(node) ((node)->cons.car)
#define CDR(node) ((node)->cons.cdr)


// todo - ahead/peak
// todo - lexer struct
// todo - result struct

enum TokenType {
  NUM,
  STRING,
  BRACK_OPEN,
  BRACK_CLOSE,
  ADD,
  SUB,
  DIV,
  MUL,

  SWITCH,
  CASE,
  
  ASSIGN,
  LAMBDA,
  IF,
  WHILE,

  INVALID,
  END_LINE
};




struct Token {
  enum TokenType type;
  int val;
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

struct Item{
  char* name;
  enum TokenType token;
};


struct Parser{
  int curPos;
  struct Token* tokens;
  struct Node* ast;
};






void error(char* message);
const char* enumToString(enum TokenType type);

struct Token peak(struct Parser* parser);
struct Token before(struct Parser* parser);
struct Token eat(struct Parser* parser);

struct Node*  parse(struct Parser* parser);
struct Node* parseSExpression(struct Parser* parser);
struct Node*  parseAtom(struct Parser* parser);
struct Node* createCons(struct Node* nodecar, struct  Node* nodecdr);

void freeNode(struct Node* node);
void printNode(struct Node* node);

struct Token createOpToken(enum TokenType type);
struct Token createIntToken(int val);
struct Token* lexer  (char* input);

int eval(struct Node* node, int* res);
int evalSExpression(struct Node*, int* res);
int evalIF(struct Node* node, int* res);
int evalWhile(struct Node* node, int* res);
int evalSwitch(struct Node* node, int* res);
int evalPredicate(struct Node* pred, int* res);

struct Token inHashTable (char* string);
const char* enumToString(enum TokenType type);

struct Token inHashTable (char* string){ // todo - make into proper hashtable
  struct Item items[]   = { {"if", IF}, {"while",  WHILE}, {"lambda", LAMBDA}, {":=", ASSIGN}, {"switch", SWITCH}, {"case", CASE}};  // todo - move assign to case
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

  


// todo - change aprouch as won't work for user defined
const char* enumToString(enum TokenType type) {
  switch(type){
  case NUM:
    return "num";
  case STRING:
    return "string";
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



int main(){
  /* char input[] = "+++ \n"; */
  /* char input[] = "(+ 2 (+ 3 4))"; */
  /* char input[] = "(+ 2 3)"; */
  /* char input[] = "(/ 8 2  2 2)"; */
  /* char input[] = "(* 2 2  2 2)"; */
  /* char input[] = "(- 4 2)"; */
  /* char input[] = "(+ 5  (- 3 (* 1 1)))"; */
  /* char input[] = "(/ 8 2  2 (+ 5  (- 3 (* 1 1))))"; */
  /* char input[] = "(+ 25 35)"; */
  /* char input[] = "(lambda 25 35)"; */
  /* char input[] = "(if 0 1 2)"; */
  /* char input[] = "(if 1 1 2)"; */
  /* char input[] = "(if (- 1 1) 1 2)"; */
  char input[] = "(switch 3 (case 1 2) (case 3 4))";
  /* char input[] = "(while 0 (+ 1 2))"; */ // need assignment first




  printf("input:\n %s\n\n",input);

  struct Token* tokens = lexer(input);
  printf("tokens:\n");
  for (int i = 0; i < MAXTOKENS && tokens[i].type != END_LINE ;i++){
    printf("%s ", enumToString(tokens[i].type));

  }
  printf("\n\n");
  struct Parser parser;
  parser.tokens = tokens;
  parser.curPos = 0;
  parser.ast = parse(&parser);
  printf("\n\nas cons:\n");
  printNode(parser.ast);
  printf("\n\n");
  int result;
  int err = eval(parser.ast, &result);
  printf("error status: %d\n", err);
  printf("\nresult %d\n",result);


  freeNode(parser.ast);
  free(parser.tokens);

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

  
void error(char* message){
  printf("%s\n",message);
  exit(1);
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
        while (isalnum(*end)){
          end++;          
        }
        size_t length = end - start;
        char *result = malloc(length + 1);
        memcpy(result,start,length);
        result[length] = '\0';
        
        struct Token token = inHashTable(result);
        free(result);
        i  = i + length;
        if (!(token.type == INVALID)){
          tokens[cur_token++] = token;

        }

        else{
          printf("unknown character %c \n",c);
          error("lex error");
        }
    }
    }
  }
  // for other words make the case a word carries on till there is space or a bracket
  tokens[cur_token].type = END_LINE;
  return tokens;
}

struct Token eat(struct Parser* parser){    //fix this check if at end before eating
  struct Token cur_token = parser->tokens[parser->curPos];
  if (parser->curPos < MAXTOKENS && cur_token.type != END_LINE){
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





struct Node*  parse(struct Parser* parser){
  printf("parser:\n");
  return parseSExpression(parser);
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




 
int evalSExpression(struct Node* node, int* res){
  if (node == NULL){
    return 1;  
  } 
  if (!node->is_atom){
    enum TokenType op = CAR(node)->atom.type;

    switch(op)
      {
      case IF:
        return evalIF(CDR(node), res);
      case WHILE:
        return evalWhile(CDR(node), res);
      case SWITCH:
        return evalSwitch(CDR(node),res);
      case ADD:
        *res = 0; break;
      case MUL:
        *res = 1; break;
      case DIV:
      case SUB:
        node = CDR(node);
        evalSExpression(CAR(node), res);
        break;
      default: return 1;

      }

    node = CDR(node);
    while(node){      
      int val;
      evalSExpression(CAR(node), &val);
      printf("%d",val);
      printf("%d\n",*res);

      switch (op)
        {
        case ADD: *res = *res + val; break;
        case SUB: *res = *res - val; break;
        case MUL: *res = *res * val; break;
        case DIV:  *res = *res / val; break;
        default: break;
        }


      node = CDR(node);
    }
  }else{
    *res= node->atom.val; 
  }
  return 0;
}


int evalPredicate(struct Node* node, int* res){
  struct Node* predictate = CAR(node);
  if (!predictate->is_atom){
    evalSExpression(predictate,res);
  }else{
    *res = predictate->atom.val;
  }
  return 0;
}


int evalIF(struct Node* node, int* res){
  int pred_val;                      
  if (evalPredicate(CAR(node),&pred_val) == 1) return 1;

  struct Node* branch = CDR(node);
  struct Node* seq = CAR(branch); 
  struct Node* alt = CAR(CDR(branch)); // todo - error check
  
  if (pred_val == 0){
    return evalSExpression(seq,res);

  }else{
    return  evalSExpression(alt,res);
  }
  
}

int evalSwitch(struct Node* node, int* res){
  int switch_val;
  evalSExpression(CAR(node),&switch_val);
  node = CDR(node);
  
  while (node != NULL){ // todo - or default case
    struct Node* cur_case = CAR(CDR(CAR(node))); // as first child will be keyword case
    struct Node* case_expr = CAR(CDR(CDR(CAR(node))));

    int case_val;
    evalSExpression(cur_case,&case_val);
    
    if (case_val == switch_val){
      return evalSExpression(case_expr, res);
    }
    node= CDR(node);
  }
  return 1;
}



int evalWhile(struct Node* node, int* res){
  int pred_val;                      
  if (evalPredicate(CAR(node), &pred_val) == 1) return 1;
  struct Node* seq =  CAR(node);

  while (pred_val == 0){
    printf("pre_val: %d \n", pred_val);
    evalSExpression(seq,res);
    if (evalPredicate(CAR(node), &pred_val) == 1) return 1;
  }
  return 0;
}



  



struct Node* parseAtom(struct Parser* parser){
  printf("%s ",enumToString(peak(parser).type));
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

int eval(struct Node* node, int* res){
  printf("semantics:\n");
  return evalSExpression(node, res);
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



/* (cons + 1 */
/* (cons 1 (cons 2 3)) */



//free all children of node



/* + -> 2 -> + -> 3 -> 4  */
/* + -> 3 ->4 */


/* + -> 2   */
/*   -> + -> 3  */
/*        -> 4   */
         
/* + -> 3  */
/*   -> 4   */

  
  
    

  
  







      
    








