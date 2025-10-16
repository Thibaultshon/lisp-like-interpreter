#include "../external/unity/unity.h"

#include "util.h"
#include "env.h"
#include "tokenize.h"
#include "parser.h"
#include "eval.h"

struct Parser parser;


void setUp(){
  g_env = NULL;
}

void tearDown(){
  freeNode(parser.ast);
  freeEnv();
}


int interpret(char* input){
  int result;
  g_env = NULL;
  parser.curPos = 0;
  while (peek(&parser,input).type != END_LINE){
    parser.ast = parse(&parser,input);
    eval(parser.ast, &result);
  }
  return result;
}



void testArithmeticOperators(){
  int res;
  res = interpret("(+ 2 (+ 3 4))");
  TEST_ASSERT_EQUAL(9,res);
  res = interpret("(+ 2 3)");
  TEST_ASSERT_EQUAL(5,res);
  res = interpret("(/ 8 2  2 2)");
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(* 2 2  2 2)");
  TEST_ASSERT_EQUAL(16,res);
  res = interpret("(- 4 2)");
  TEST_ASSERT_EQUAL(2,res);
  res = interpret("(+ 5  (- 3 (* 1 1)))");
  TEST_ASSERT_EQUAL(7,res);
  res = interpret("(+ 25 35)");
  TEST_ASSERT_EQUAL(60,res);
  res = interpret("(/ 8 2  2 (+ 5  (- 3 (* 1 1))))");
  TEST_ASSERT_EQUAL(0,res);
}

void testIfStatements(){
  int res;
  res = interpret("(if 0 1 2)");
  TEST_ASSERT_EQUAL(2,res);
  res = interpret("(if 1 1 2)");
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(if (- 1 1) 1 2)");
  TEST_ASSERT_EQUAL(2,res);
  res = interpret("(if (- 2 1) 1 2)");
  TEST_ASSERT_EQUAL(1,res);

}

void testSwitchStatements(){
  int res;
  res = interpret("(switch 3 (case 1 2) (case 3 4))");
  TEST_ASSERT_EQUAL(4,res);
  res = interpret("(switch 1 (case 1 2) (case 3 4))");
  TEST_ASSERT_EQUAL(2,res);
}

void testAssignStatements(){
  int res;
  res = interpret("(+ (:= x 4) 1)");
  TEST_ASSERT_EQUAL(5,res);
  res = interpret("(:= x 4)");
  TEST_ASSERT_EQUAL(4,res);
  res = interpret("(:= x 3) x");
  TEST_ASSERT_EQUAL(3,res);
  res = interpret("(:= x 3) (:= y 2) (+ 4 x)");
  TEST_ASSERT_EQUAL(7,res);
}

void testWhileLoop(){
    int res;
    res = interpret("(:= x 1) (:= y 3) (while x (:= y (+ y 1)) (:= x 0))  y");
    TEST_ASSERT_EQUAL(4,res);
    res = interpret("(:= x 1) (:= y 0) (while x (:= x 0) ) x");
    TEST_ASSERT_EQUAL(0,res);
    res = interpret("(:= x 1) (:= y 5) (while x (:= y  (+ y 1)) (:= x 0) y)");
    TEST_ASSERT_EQUAL(6,res);
    res = interpret("(:= x 1) (:= y 5) (while x (:= y  (+ y 1)) (:= x 0)) y");
    TEST_ASSERT_EQUAL(6,res);
    res = interpret("(:= x 1) (:= y 0) (while x (:= y  (+ y 1)) (:= d 3) (:= x 0) y)");
    TEST_ASSERT_EQUAL(1,res);
    res = interpret("(:= x 1) (:= y 0) (while x (:= y  (+ y 1)) (:= x 0) )");
    TEST_ASSERT_EQUAL(0,res);
    res = interpret("(:= x 5) (:= y 1) (while x (:= x  (- x 1)) (:= y (+ y 1) )) y");
    TEST_ASSERT_EQUAL(6,res);
}


void testRelationalOperators(){
  int res;
  res = interpret("(> 1)");    
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(< 1)");    
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(> 1 2)");    
  TEST_ASSERT_EQUAL(0,res);
  res = interpret("(> 2 1)");    
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(< 2 1)");    
  TEST_ASSERT_EQUAL(0,res);
  res = interpret("(< 1 2)");    
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(< 1 2 3 4 5)");    
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(> 1 4 3 4 5)");    
  TEST_ASSERT_EQUAL(0,res);

  res = interpret("(= 1)");    
  TEST_ASSERT_EQUAL(1 ,res);
  res = interpret("(/= 1)");    
  TEST_ASSERT_EQUAL(1 ,res);  
  res = interpret("(= 1 1 1 1)");    
  TEST_ASSERT_EQUAL(1 ,res);
  res = interpret("(= 1 2 1 1)");    
  TEST_ASSERT_EQUAL(0 ,res);
  res = interpret("(= 1 2 1 3)");    
  TEST_ASSERT_EQUAL(0 ,res);
  res = interpret("(/= 1 1 1 1)");    
  TEST_ASSERT_EQUAL(0 ,res);
  res = interpret("(/= 1 2 1 3)");    
  TEST_ASSERT_EQUAL(1 ,res);


}

void testNewFeature(){
  //// to implement

  char input[]= "(:= x 4)";
  printf("\ninput:\n%s\n\n",input);
  

  int result;
  int err;

  parser.curPos = 0;
  while (peek(&parser,input).type != END_LINE){
    //////Parse
    printf("\n\nParser:\n");
    parser.ast = parse(&parser,input);
    printNode(parser.ast);
    printf("\n");
    
    ////    Eval
    printf("\nSemantics:\n");
    err = eval(parser.ast, &result);
    printf("error status: %d\n", err);
    printf("result: %d\n",result);
  }

}




int main(){
  UNITY_BEGIN();
  RUN_TEST(testArithmeticOperators);
  RUN_TEST(testIfStatements);
  RUN_TEST(testSwitchStatements);
  RUN_TEST(testAssignStatements);
  RUN_TEST(testWhileLoop);
  RUN_TEST(testRelationalOperators);

  /* RUN_TEST(testNewFeature); */
  return UNITY_END();
}
