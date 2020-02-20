#include "platon/print.hpp"
#include "unit_test.hpp"
#include <stdlib.h>

using namespace platon;

std::string test_string = "string test";

TEST_CASE(print, string) {
  println(test_string);
  ASSERT_EQ(test_string, std::string("string test"));
}

TEST_CASE(print, malloc) {
    // malloc
    int * p_int = (int *)malloc(sizeof(int));
    *p_int = -1;
    println(*p_int);
    println(int(p_int));
    ASSERT_EQ(*p_int, -1);
    free(p_int);

    // memeset
    p_int = (int *)malloc(sizeof(int));
    memset(p_int, 0xff, sizeof(int));
    println(*p_int);
    println(int(p_int));
    ASSERT_EQ(*p_int, -1);
    free(p_int);

    // memcpy
    p_int = (int *)malloc(sizeof(int));
    int temp = -1;
    memcpy(p_int, &temp, sizeof(int));
    println(*p_int);
    println(int(p_int));
    ASSERT_EQ(*p_int, -1);
    free(p_int);

    // realloc
    p_int = (int *)malloc(5*sizeof(int));
    for (int i = 0; i < 5; i++){
        p_int[i] = i;
    }
    println(int(p_int));
    int *p_new_int=(int *)realloc(p_int, 10*sizeof(int));
    for (int i = 5; i < 10; i++){
        p_new_int[i] = i;
    }
    for (int i = 0; i < 10; i++){
        println(p_new_int[i]);
        ASSERT_EQ(p_new_int[i], i);
    }
    println(int(p_new_int));
    free(p_new_int);

    // calloc
    p_int = (int*)calloc(5, sizeof(int));
    for (int i = 0; i < 5; i++){
        println(p_int[i]);
        ASSERT_EQ(p_int[i], 0);
    }
    for (int i = 0; i < 5; i++){
        p_int[i] = i;
    }
    for (int i = 0; i < 5; i++){
        println(p_int[i]);
        ASSERT_EQ(p_int[i], i);
    }
    println(int(p_int));
    free(p_int);

    // malloc
    for(int i = 0; i < 16; i ++){
      int * p_int = (int *)malloc(1024 * 64);
      println(int(p_int));
      free(p_int);
    }
}

std::vector<std::string> vect_a =  {"ad", "cd"};
std::vector<std::string> vect_b =  {"ef", "gh"};
std::vector<std::string> vect_c =  {"ig", "kl"};

TEST_CASE(print, global){
  println(int(vect_a.data()));
  for (auto i : vect_a) {
    println(i);
  }
  std::vector<std::string> temp_a = {"ad", "cd"};
  ASSERT_EQ(vect_a, temp_a);

  println(int(vect_b.data()));
  for (auto i : vect_b) {
    println(i);
  }
  std::vector<std::string> temp_b = {"ef", "gh"};
  ASSERT_EQ(vect_b, temp_b);

  println(int(vect_c.data()));
  for (auto i : vect_c) {
    println(i);
  }
  std::vector<std::string> temp_c = {"ig", "kl"};
  ASSERT_EQ(vect_c, temp_c);

  std::vector<std::string> vect_d =  {"mn", "op"};
  println(int(vect_d.data()));
  for (auto i : vect_d) {
    println(i);
  }
  std::vector<std::string> temp_d = {"mn", "op"};
  ASSERT_EQ(vect_d, temp_d);

  std::vector<std::string> vect_e =  {"qr", "st"};
  println(int(vect_e.data()));
  for (auto i : vect_e) {
    println(i);
  }
  std::vector<std::string> temp_e = {"qr", "st"};
  ASSERT_EQ(vect_e, temp_e);
}

class TestType 
{
    public:
      TestType(){ count = 0;}
      int get_number(){
        int temp = count;
        ++count;
        vect_count.push_back(temp);
        return temp;
      }
      int get_size(){
        return vect_count.size();
      }
    public:
      int count;
      std::vector<int> vect_count;
};

TestType my_test;
int count_1 = my_test.get_number();
int count_2 = my_test.get_number();
int count_3 = my_test.get_number();

TEST_CASE(print, count){
  int count_4 = my_test.get_number();
  int count_5 = my_test.get_number();
  ASSERT_EQ(count_1, 0);
  ASSERT_EQ(count_2, 1);
  ASSERT_EQ(count_3, 2);
  ASSERT_EQ(count_4, 3);
  ASSERT_EQ(count_5, 4);
  ASSERT_EQ(my_test.get_size(), 5);
}

UNITTEST_MAIN() {
  RUN_TEST(print, string);
  RUN_TEST(print, malloc);
  RUN_TEST(print, global);
  RUN_TEST(print, count);
}