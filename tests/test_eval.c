#include "../external/unity/unity.h"

#include "util.h"
#include "env.h"
#include "tokenize.h"
#include "parser.h"
#include "eval.h"

struct Token* tokens;
struct Parser parser;


void setUp(){
  g_env = NULL;
}

void tearDown(){
  freeTokens(parser.tokens);
  freeNode(parser.ast);     
  freeEnv();
}


int interpret(char* input){
  int result;
  tokens= lexer(input);
  parser.tokens = tokens;
  parser.curPos = 0;
  parser.ast = parse(&parser);
  eval(parser.ast, &result);
  return result;
}



void testArithmeticOperations(){
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
  /* char input[] = "(/ 8 2  2 (+ 5  (- 3 (* 1 1))))";  */ // check                          
}

void testIfStatements(){
  int res;
  res = interpret("(if 0 1 2)");
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(if 1 1 2)");
  TEST_ASSERT_EQUAL(2,res);
  res = interpret("(if (- 1 1) 1 2)");
  TEST_ASSERT_EQUAL(1,res);
  res = interpret("(if (- 2 1) 1 2)");
  TEST_ASSERT_EQUAL(2,res);

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
}


int main(){
  UNITY_BEGIN();
  RUN_TEST(testArithmeticOperations);
  RUN_TEST(testIfStatements);
  RUN_TEST(testSwitchStatements);
  RUN_TEST(testAssignStatements);
  return UNITY_END();
}
