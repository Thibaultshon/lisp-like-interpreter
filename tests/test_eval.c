#include "../external/unity/unity.h"
#include <stdbool.h>

#include "util.h"
#include "env.h"
#include "tokenize.h"
#include "parser.h"
#include "eval.h"
#include "interpreter.h"



void setUp(){
}

void tearDown(){
}


int testInterpret(char* input, struct Result* result){
  struct NodeRegistry registry = {
    .max = 10,
    .count = 0,
    .nodes = malloc(10 * sizeof(struct Node*))
  };
  g_env = enterEnv(NULL); 
  interpret(input,result,&registry,false);
  freeRegistry(&registry);
  g_env = leaveEnv(g_env);
  return 0;
}




void testArithmeticOperators(){
  struct Result res;
  testInterpret("(+ 2 (+ 3 4))",&res);
  TEST_ASSERT_EQUAL(9,resultGetInt(&res));
  testInterpret("(+ 2 3)",&res);
  TEST_ASSERT_EQUAL(5,resultGetInt(&res));
  testInterpret("(/ 8 2  2 2)",&res);
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  testInterpret("(* 2 2  2 2)",&res);
  TEST_ASSERT_EQUAL(16,resultGetInt(&res));
  testInterpret("(- 4 2)",&res);
  TEST_ASSERT_EQUAL(2,resultGetInt(&res));
  testInterpret("(+ 5  (- 3 (* 1 1)))",&res);
  TEST_ASSERT_EQUAL(7,resultGetInt(&res));
  testInterpret("(+ 25 35)",&res);
  TEST_ASSERT_EQUAL(60,resultGetInt(&res));
  testInterpret("(/ 8 2  2 (+ 5  (- 3 (* 1 1))))",&res);
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
}

void testIfStatements(){
  struct Result res;
  testInterpret("(if 0 1 2)",&res);
  TEST_ASSERT_EQUAL(2,resultGetInt(&res));
  testInterpret("(if 1 1 2)",&res);
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  testInterpret("(if (- 1 1) 1 2)",&res);
  TEST_ASSERT_EQUAL(2,resultGetInt(&res));
  testInterpret("(if (- 2 1) 1 2)",&res);
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));

}

void testSwitchStatements(){
  struct Result res;
  testInterpret("(switch 3 (case 1 2) (case 3 4))",&res);
  TEST_ASSERT_EQUAL(4,resultGetInt(&res));
  testInterpret("(switch 1 (case 1 2) (case 3 4))",&res);
  TEST_ASSERT_EQUAL(2,resultGetInt(&res));
}

void testAssignStatements(){
  struct  Result res;
  testInterpret("(+ (:= x 4) 1)",&res);
  TEST_ASSERT_EQUAL(5,resultGetInt(&res));
  testInterpret("(:= x 4)",&res);
  TEST_ASSERT_EQUAL(4,resultGetInt(&res));
  testInterpret("(:= x 3) x",&res);
  TEST_ASSERT_EQUAL(3,resultGetInt(&res));
  testInterpret("(:= x 3) (:= y 2) (+ 4 x)",&res);
  TEST_ASSERT_EQUAL(7,resultGetInt(&res));
}

void testWhileLoop(){
  struct Result res;
  testInterpret("(:= x 1) (:= y 3) (while x (:= y (+ y 1)) (:= x 0))  y",&res);
  TEST_ASSERT_EQUAL(4,resultGetInt(&res));
  testInterpret("(:= x 1) (:= y 0) (while x (:= x 0) ) x",&res);
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  testInterpret("(:= x 1) (:= y 5) (while x (:= y  (+ y 1)) (:= x 0) y)",&res);
  TEST_ASSERT_EQUAL(6,resultGetInt(&res));
  testInterpret("(:= x 1) (:= y 5) (while x (:= y  (+ y 1)) (:= x 0)) y",&res);
  TEST_ASSERT_EQUAL(6,resultGetInt(&res));
  testInterpret("(:= x 1) (:= y 0) (while x (:= y  (+ y 1)) (:= d 3) (:= x 0) y)",&res);
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  testInterpret("(:= x 1) (:= y 0) (while x (:= y  (+ y 1)) (:= x 0) )",&res);
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  testInterpret("(:= x 5) (:= y 1) (while x (:= x  (- x 1)) (:= y (+ y 1) )) y",&res);
  TEST_ASSERT_EQUAL(6,resultGetInt(&res));
}


void testRelationalOperators(){
  struct Result res;
  testInterpret("(> 1)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  testInterpret("(< 1)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  testInterpret("(> 1 2)",&res);    
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  testInterpret("(> 2 1)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  testInterpret("(< 2 1)",&res);    
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  testInterpret("(< 1 2)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  testInterpret("(< 1 2 3 4 5)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  testInterpret("(> 1 4 3 4 5)",&res);    
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));

  testInterpret("(= 1)",&res);    
  TEST_ASSERT_EQUAL(1 ,resultGetInt(&res));
  testInterpret("(/= 1)",&res);    
  TEST_ASSERT_EQUAL(1 ,resultGetInt(&res));  
  testInterpret("(= 1 1 1 1)",&res);    
  TEST_ASSERT_EQUAL(1 ,resultGetInt(&res));
  testInterpret("(= 1 2 1 1)",&res);    
  TEST_ASSERT_EQUAL(0 ,resultGetInt(&res));
  testInterpret("(= 1 2 1 3)",&res);    
  TEST_ASSERT_EQUAL(0 ,resultGetInt(&res));
  testInterpret("(/= 1 1 1 1)",&res);    
  TEST_ASSERT_EQUAL(0 ,resultGetInt(&res));
  testInterpret("(/= 1 2 1 3)",&res);    
  TEST_ASSERT_EQUAL(1 ,resultGetInt(&res));


}

void testLexicalScoping(){
  struct Result res;
  testInterpret("(let ((x 2)) x)",&res);    
  TEST_ASSERT_EQUAL(2, resultGetInt(&res));
  testInterpret("(let ((x 2)))",&res);
  TEST_ASSERT_EQUAL(0, resultGetInt(&res));
  testInterpret("(let ((x 2) (y 3)) x)",&res);
  TEST_ASSERT_EQUAL(2, resultGetInt(&res));
  testInterpret("(let ((x 2) (y 3)) y)",&res);    
  TEST_ASSERT_EQUAL(3, resultGetInt(&res));
  testInterpret("(let ((x 2) (y 3)) (:= y (+  1 x)) (:= x (+ 1 y)) x)",&res);
  TEST_ASSERT_EQUAL(4, resultGetInt(&res));
  testInterpret("(:= x 1) (let ((x 2)  (y 3))) x",&res);
  TEST_ASSERT_EQUAL(1, resultGetInt(&res));
  testInterpret("(:= x 1) (let ((y 2)) (:= x 5)) x",&res);
  TEST_ASSERT_EQUAL(5, resultGetInt(&res));
  testInterpret("(:= x 1)  (+ (let ((x 2)) x ) x)",&res);    
  TEST_ASSERT_EQUAL(3, resultGetInt(&res));
  testInterpret("(:= x 1) (let ((x 2)) (:= x 5)) x",&res);    
  TEST_ASSERT_EQUAL(1, resultGetInt(&res));
  testInterpret("(let ((x 2)) (let ((x 4)) x))",&res);    
  TEST_ASSERT_EQUAL(4, resultGetInt(&res));
  testInterpret("(:= y 5 ) (let ((x 2)) y)",&res);
  TEST_ASSERT_EQUAL(5, resultGetInt(&res));
}


void testCallingLambdaFunctions(){
  struct Result res;
  testInterpret("((lambda (x y) (+ x y)) 1 2)",&res);    
  TEST_ASSERT_EQUAL(3, resultGetInt(&res));
  testInterpret("(:= x 2) ((lambda (y) (+ x y)) 3)",&res);
  TEST_ASSERT_EQUAL(5, resultGetInt(&res));
  testInterpret("((lambda () (+ 2 2)))",&res);
  TEST_ASSERT_EQUAL(4, resultGetInt(&res));
  testInterpret("((lambda (x) ) 5)",&res);
  TEST_ASSERT_EQUAL(0, resultGetInt(&res));
  testInterpret("((lambda (x)() ) 1)",&res);
  TEST_ASSERT_EQUAL(0, resultGetInt(&res));
  testInterpret("(let ((addOne (lambda (x)(+ x 1)))) (addOne 3))",&res);
  TEST_ASSERT_EQUAL(4, resultGetInt(&res));
  testInterpret("((lambda (x) (x 4)) (lambda (x) (+ x 1)))",&res);
  TEST_ASSERT_EQUAL(5, resultGetInt(&res));
}

void testCallingNamedFunctions(){
  struct Result res;
  testInterpret(
    "(:= test (lambda (x)"
           "     (+ x 1)))"
    "(test 3)",&res);
  TEST_ASSERT_EQUAL(4,resultGetInt(&res));
  testInterpret(
    "(:= twoPlus3 (lambda ()"
                "     (+ 2 3)))"
    "(twoPlus3)",&res);
  TEST_ASSERT_EQUAL(5,resultGetInt(&res));
  testInterpret(
     "(:= test (lambda (x y )"
            "     (+ x y)))"
     "(test 5 6)",&res);
  TEST_ASSERT_EQUAL(11,resultGetInt(&res));
  testInterpret(
    "(:= test (lambda ()))"
    "(test)",&res);
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  testInterpret(
    "(:= y 5)"
    "(:= test (lambda (x)"
    "           (+ x y))) "
    "(test 3)",&res);
  TEST_ASSERT_EQUAL(8,resultGetInt(&res));
  testInterpret(
    "(:= double (lambda (x)"
              "(:= x (* x 2))))"
    "(double 4)",&res);
  TEST_ASSERT_EQUAL(8,resultGetInt(&res));
  testInterpret(
     "(:= multiBody (lambda (x y )"
               "     (:= x (+ x y))"
                "     (:= y (+ y x))))"
     "(multiBody 5 6)",&res);
  TEST_ASSERT_EQUAL(17,resultGetInt(&res));
  testInterpret(
    "(:= plusOne (lambda (x) (+ x 1)))"
    "(:= multiBody (lambda (x y )"
                 "     (:= x (+ x y))"
                 "     (:= y (+ y x))))"
     "(multiBody (plusOne 2) 6)",&res);
  TEST_ASSERT_EQUAL(15,resultGetInt(&res));


  testInterpret(
            "(:= recursion (lambda (x) (if (> x 0) (recursion (:= x (- x 1))) 9)))"
            "(recursion 10)" ,&res);
  TEST_ASSERT_EQUAL(9,resultGetInt(&res));
}


void testsSemanticErrors(){
  struct Result res;
  testInterpret("(switch 1 (case 4 2) (case 3 4))", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: no case matching predicate", resultGetError(&res));
  testInterpret("(5 2)", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: missing function at head", resultGetError(&res));
  testInterpret("(:= x 2)(x 2)", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: missing function at head", resultGetError(&res));
  testInterpret("(:= 4 2)", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: id not a symbol", resultGetError(&res));
  testInterpret("(:= x -)", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: unable to use value type in assignment", resultGetError(&res));
  testInterpret("x", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: undefined variable", resultGetError(&res));
  testInterpret("(let ((x 2)) y)", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: undefined variable", resultGetError(&res));

  testInterpret("((lambda (x y z) (+ x 1)) 3)", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: malformed statement", resultGetError(&res));
  testInterpret("(switch () 3)", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: malformed statement", resultGetError(&res));
  testInterpret("(switch 1 2)", &res);
  TEST_ASSERT_EQUAL_STRING("ERROR: malformed statement", resultGetError(&res));
}

void testsParseErrors(){
  struct Result res;
  testInterpret("(- 1 2", &res);  //store enum not string hten convert with string
  TEST_ASSERT_EQUAL_STRING("Parsing Error: malformed list", resultGetError(&res));
  testInterpret("£", &res);
  TEST_ASSERT_EQUAL_STRING("Lexical Error: not valid lexeme", resultGetError(&res));
  testInterpret(")", &res); 
  TEST_ASSERT_EQUAL_STRING("Parsing Error: malformed list", resultGetError(&res));
  testInterpret("(", &res); 
  TEST_ASSERT_EQUAL_STRING("Parsing Error: malformed list", resultGetError(&res));
  testInterpret(") 1", &res); 
  TEST_ASSERT_EQUAL_STRING("Parsing Error: malformed list", resultGetError(&res));
  testInterpret("(:= x 2) ((lambda (y) (+ x y)) 3", &res); 
  TEST_ASSERT_EQUAL_STRING("Parsing Error: malformed list", resultGetError(&res));
  //(lambda 2 3)  (x:= 2)
}


void testNewFeature(){
  //// to implement
  struct Parser parser;
  g_env = enterEnv(NULL);
  /* char input[]= ""; */
  char input[]= "(:= x 1) (let ((x 2)) (:= x 5)) x";
  /* char input[]= ")"; */

  printf("\ninput:\n%s\n",input);


  printf("\n\nLexer:\n");
  printStringToTokens(input);
  
  initParser(&parser);

  struct NodeRegistry registry = {
    .max = 10,
    .count = 0,
    .nodes = malloc(10 * sizeof(struct Node*))
  };


  while (peek(&parser,input).type != TOK_END_LINE){
           /* Parse */


    printf("\n\nParser:\n");
    struct Node* ast = NULL;
    enum ParseResultType status = parse(&parser,input,&ast);


    printNode(ast);
    
          /* Eval */
    printf("\n\nSemantics:\n");
    struct Result result;
    int err;
    if (status == PARSE_RESULT_SUCCESS){
    err = eval(ast, &result);
    printf("error status: %d\n", err);
    printf("\nMappings:");
    printEnvironments(g_env);
    
    }else{
      //parsing error
      result.type = RESULT_ERROR;
      result.symbol_val = (char*)parseTypeToString(status);
    }

    printf("\nresult:");
    printResult(&result);
    printf("\n");


    registerNodes(&registry,ast);
    if (status != PARSE_RESULT_SUCCESS) break;
    
  }
  

  freeRegistry(&registry);
  freeParser(&parser);
  g_env = leaveEnv(g_env);

}


int main(){
  UNITY_BEGIN();

  RUN_TEST(testArithmeticOperators);
  RUN_TEST(testIfStatements);
  RUN_TEST(testSwitchStatements);
  RUN_TEST(testAssignStatements);
  RUN_TEST(testWhileLoop);
  RUN_TEST(testRelationalOperators);
  RUN_TEST(testLexicalScoping);
  RUN_TEST(testCallingLambdaFunctions);
  RUN_TEST(testCallingNamedFunctions);
  RUN_TEST(testsSemanticErrors);
  RUN_TEST(testsParseErrors);
  /* RUN_TEST(testNewFeature); */
  return UNITY_END();
}
