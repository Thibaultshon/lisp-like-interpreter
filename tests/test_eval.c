#include "../external/unity/unity.h"

#include "util.h"
#include "env.h"
#include "tokenize.h"
#include "parser.h"
#include "eval.h"




void setUp(){
}

void tearDown(){
}


int interpret(char* input, struct Result* result){
  struct Parser parser;
  g_env = enterEnv(NULL);
  initParser(&parser);
  int max_nodes = 10;
  size_t node_count =0; 
  struct Node** used_nodes = malloc(max_nodes*sizeof(*used_nodes));

  while (peek(&parser,input).type != TOK_END_LINE){
    struct Node* ast = parse(&parser,input);
    eval(ast, result);

    used_nodes[node_count] = ast;
    node_count++;
     if (node_count > max_nodes){
       max_nodes +=10; //todo - tmp until add in lambda structs that copy node
       used_nodes = realloc(used_nodes,max_nodes*sizeof(**used_nodes));
    }
  }

  for (int i =0; i <node_count;i++){
    freeNode(used_nodes[i]);
  }
  free(used_nodes);
  freeParser(&parser);
  g_env = leaveEnv(g_env);
  return 0;
}




void testArithmeticOperators(){
  struct Result res;
  interpret("(+ 2 (+ 3 4))",&res);
  TEST_ASSERT_EQUAL(9,resultGetInt(&res));
  interpret("(+ 2 3)",&res);
  TEST_ASSERT_EQUAL(5,resultGetInt(&res));
  interpret("(/ 8 2  2 2)",&res);
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  interpret("(* 2 2  2 2)",&res);
  TEST_ASSERT_EQUAL(16,resultGetInt(&res));
  interpret("(- 4 2)",&res);
  TEST_ASSERT_EQUAL(2,resultGetInt(&res));
  interpret("(+ 5  (- 3 (* 1 1)))",&res);
  TEST_ASSERT_EQUAL(7,resultGetInt(&res));
  interpret("(+ 25 35)",&res);
  TEST_ASSERT_EQUAL(60,resultGetInt(&res));
  interpret("(/ 8 2  2 (+ 5  (- 3 (* 1 1))))",&res);
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
}

void testIfStatements(){
  struct Result res;
  interpret("(if 0 1 2)",&res);
  TEST_ASSERT_EQUAL(2,resultGetInt(&res));
  interpret("(if 1 1 2)",&res);
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  interpret("(if (- 1 1) 1 2)",&res);
  TEST_ASSERT_EQUAL(2,resultGetInt(&res));
  interpret("(if (- 2 1) 1 2)",&res);
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));

}

void testSwitchStatements(){
  struct Result res;
  interpret("(switch 3 (case 1 2) (case 3 4))",&res);
  TEST_ASSERT_EQUAL(4,resultGetInt(&res));
  interpret("(switch 1 (case 1 2) (case 3 4))",&res);
  TEST_ASSERT_EQUAL(2,resultGetInt(&res));
}

void testAssignStatements(){
  struct  Result res;
  interpret("(+ (:= x 4) 1)",&res);
  TEST_ASSERT_EQUAL(5,resultGetInt(&res));
  interpret("(:= x 4)",&res);
  TEST_ASSERT_EQUAL(4,resultGetInt(&res));
  interpret("(:= x 3) x",&res);
  TEST_ASSERT_EQUAL(3,resultGetInt(&res));
  interpret("(:= x 3) (:= y 2) (+ 4 x)",&res);
  TEST_ASSERT_EQUAL(7,resultGetInt(&res));
}

void testWhileLoop(){
  struct Result res;
  interpret("(:= x 1) (:= y 3) (while x (:= y (+ y 1)) (:= x 0))  y",&res);
  TEST_ASSERT_EQUAL(4,resultGetInt(&res));
  interpret("(:= x 1) (:= y 0) (while x (:= x 0) ) x",&res);
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  interpret("(:= x 1) (:= y 5) (while x (:= y  (+ y 1)) (:= x 0) y)",&res);
  TEST_ASSERT_EQUAL(6,resultGetInt(&res));
  interpret("(:= x 1) (:= y 5) (while x (:= y  (+ y 1)) (:= x 0)) y",&res);
  TEST_ASSERT_EQUAL(6,resultGetInt(&res));
  interpret("(:= x 1) (:= y 0) (while x (:= y  (+ y 1)) (:= d 3) (:= x 0) y)",&res);
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  interpret("(:= x 1) (:= y 0) (while x (:= y  (+ y 1)) (:= x 0) )",&res);
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  interpret("(:= x 5) (:= y 1) (while x (:= x  (- x 1)) (:= y (+ y 1) )) y",&res);
  TEST_ASSERT_EQUAL(6,resultGetInt(&res));
}


void testRelationalOperators(){
  struct Result res;
  interpret("(> 1)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  interpret("(< 1)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  interpret("(> 1 2)",&res);    
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  interpret("(> 2 1)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  interpret("(< 2 1)",&res);    
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  interpret("(< 1 2)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  interpret("(< 1 2 3 4 5)",&res);    
  TEST_ASSERT_EQUAL(1,resultGetInt(&res));
  interpret("(> 1 4 3 4 5)",&res);    
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));

  interpret("(= 1)",&res);    
  TEST_ASSERT_EQUAL(1 ,resultGetInt(&res));
  interpret("(/= 1)",&res);    
  TEST_ASSERT_EQUAL(1 ,resultGetInt(&res));  
  interpret("(= 1 1 1 1)",&res);    
  TEST_ASSERT_EQUAL(1 ,resultGetInt(&res));
  interpret("(= 1 2 1 1)",&res);    
  TEST_ASSERT_EQUAL(0 ,resultGetInt(&res));
  interpret("(= 1 2 1 3)",&res);    
  TEST_ASSERT_EQUAL(0 ,resultGetInt(&res));
  interpret("(/= 1 1 1 1)",&res);    
  TEST_ASSERT_EQUAL(0 ,resultGetInt(&res));
  interpret("(/= 1 2 1 3)",&res);    
  TEST_ASSERT_EQUAL(1 ,resultGetInt(&res));


}

void testLexicalScoping(){
  struct Result res;
  interpret("(let ((x 2)) x)",&res);    
  TEST_ASSERT_EQUAL(2, resultGetInt(&res));
  interpret("(let ((x 2)))",&res);
  TEST_ASSERT_EQUAL(0, resultGetInt(&res));
  interpret("(let ((x 2) (y 3)) x)",&res);
  TEST_ASSERT_EQUAL(2, resultGetInt(&res));
  interpret("(let ((x 2) (y 3)) y)",&res);    
  TEST_ASSERT_EQUAL(3, resultGetInt(&res));
  interpret("(let ((x 2) (y 3)) (:= y (+  1 x)) (:= x (+ 1 y)) x)",&res);
  TEST_ASSERT_EQUAL(4, resultGetInt(&res));
  interpret("(:= x 1) (let ((x 2)  (y 3))) x",&res);
  TEST_ASSERT_EQUAL(1, resultGetInt(&res));
  interpret("(:= x 1)  (+ (let ((x 2)) x ) x)",&res);    
  TEST_ASSERT_EQUAL(3, resultGetInt(&res));
  interpret("(:= x 1) (let ((x 2)) (:= x 5)) x",&res);    
  TEST_ASSERT_EQUAL(1, resultGetInt(&res));
  interpret("(let ((x 2)) (let ((x 4)) x))",&res);    
  TEST_ASSERT_EQUAL(4, resultGetInt(&res));
  interpret("(:= y 5 ) (let ((x 2)) y)",&res);
  TEST_ASSERT_EQUAL(5, resultGetInt(&res));
}


void testCallingLambdaFunctions(){
  struct Result res;
  interpret("((lambda (x y) (+ x y)) 1 2)",&res);    
  TEST_ASSERT_EQUAL(3, resultGetInt(&res));
  interpret("(:= x 2) ((lambda (y) (+ x y)) 3)",&res);
  TEST_ASSERT_EQUAL(5, resultGetInt(&res));
  interpret("((lambda () (+ 2 2)))",&res);
  TEST_ASSERT_EQUAL(4, resultGetInt(&res));
  interpret("((lambda (x) ) 5)",&res);
  TEST_ASSERT_EQUAL(0, resultGetInt(&res));
  interpret("((lambda (x)() ) 1)",&res);
  TEST_ASSERT_EQUAL(0, resultGetInt(&res));
}

void testCallingNamedFunctions(){
  struct Result res;
  interpret(
    "(def test (x)"
    "     (+ x 1))"
    "(test 3)",&res);
  TEST_ASSERT_EQUAL(4,resultGetInt(&res));
  interpret(
    "(def twoPlus3 ()"
    "     (+ 2 3))"
    "(twoPlus3)",&res);
  TEST_ASSERT_EQUAL(5,resultGetInt(&res));
  interpret(
     "(def test (x y )"
     "     (+ x y))"
     "(test 5 6)",&res);
  TEST_ASSERT_EQUAL(11,resultGetInt(&res));
  interpret(
    "(def test ())"
    "(test)",&res);
  TEST_ASSERT_EQUAL(0,resultGetInt(&res));
  interpret(
    "(:= y 5)"
    "(def test (x)"
    "    (+ x y)) "
    "(test 3)",&res);
  TEST_ASSERT_EQUAL(8,resultGetInt(&res));
  interpret(
    "(def double (x)"
       "(:= x (* x 2)))"
    "(double 4)",&res);
  TEST_ASSERT_EQUAL(8,resultGetInt(&res));
  interpret(
     "(def multiBody (x y )"
     "     (:= x (+ x y))"
     "     (:= y (+ y x)))"
     "(multiBody 5 6)",&res);
  TEST_ASSERT_EQUAL(17,resultGetInt(&res));
  interpret(
    "(def plusOne (x) (+ x 1))"
    "(def multiBody (x y )"
     "     (:= x (+ x y))"
     "     (:= y (+ y x)))"
     "(multiBody (plusOne 2) 6)",&res);
  TEST_ASSERT_EQUAL(15,resultGetInt(&res));


  interpret(
            "(def recursion (x) (if (> x 0) (recursion (:= x (- x 1))) 9))"
            "(recursion 10)" ,&res);
  TEST_ASSERT_EQUAL(9,resultGetInt(&res));
}



void testNewFeature(){
  //// to implement
  struct Parser parser;
  g_env = enterEnv(NULL);

  char input[]= "";
  /* char input[] = "(:= x 1) (let ((x 2)) (:= x 5)) x"; */
  /* char input[] = "(def x 2) (:= x 3)"; */
  /* char input[] = "(map (lambda (x) (+ x 2 3)) 1 2 3) impelment as macro */
  printf("\ninput:\n%s\n",input);

  /* char input[]=  "(if (= 1 1) 1)" */
  printf("\n\nLexer:\n");
  printStringToTokens(input);

  initParser(&parser);
  while (peek(&parser,input).type != TOK_END_LINE){
           /* Parse */


    printf("\n\nParser:\n");
    struct Node* ast = parse(&parser, input);
    printNode(ast);
    
          /* Eval */
    printf("\n\nSemantics:\n");
    struct Result result;
    int err;
    err = eval(ast, &result);
    printf("error status: %d\n", err);
    /* printf("\nMappings:"); */
    /* printEnvironments(g_env); */

    printf("\nresult:");
    printResult(&result);
    printf("\n");

    /* freeNode(ast); */
  }
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
  /* RUN_TEST(testNewFeature); */
  return UNITY_END();
}
