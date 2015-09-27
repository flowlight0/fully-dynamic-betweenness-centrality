#include "gtest/gtest.h"
#include "algorithm/id_manager.hpp"
using namespace std;

TEST(IDMANAGER, MAKEDEAD_NOTCRUSH){
  IDManager id_manager(5);
  id_manager.MakeDead(2);
}

TEST(IDMANAGER, MAKEALIVE_NOTCRUSH){
  IDManager id_manager(5);
  id_manager.MakeDead(2);
  id_manager.MakeAlive(2);
}

TEST(IDMANAGER, ADD_NOTCRUSH){
  IDManager id_manager(5);
  id_manager.Add();
  id_manager.MakeDead(5);
}

TEST(IDManager, SAMPLE_FROM_EMPTY){
  IDManager id_manager(5);
  ASSERT_EQ(id_manager.SampleDead(), -1);
  for (int i = 0; i < 5;i++){
    id_manager.MakeDead(i);
  }
  ASSERT_EQ(id_manager.SampleAlive(), -1);
}


TEST(IDManager, MAKEALIVE_AFTER_MAKEDEAD){
  IDManager id_manager(5);
  id_manager.MakeDead(0);
  id_manager.MakeDead(4);
  id_manager.MakeAlive(0);
  vector<int> count(5);
  for (int i = 0; i < 10000; i++){
    count[id_manager.SampleAlive()]++;
  }

  ASSERT_EQ(count[4], 0);
  for (int i = 0; i < 3; i++){
    ASSERT_NEAR(count[i], 2500, 100);
  }
}

