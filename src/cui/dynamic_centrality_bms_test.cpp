#include "algorithm/centrality_brandes.hpp"
#include "algorithm/dynamic_centrality_bms.hpp"
#include "common.hpp"
#include "gtest/gtest.h"
#include <memory>
using namespace betweenness_centrality;
using namespace std;

template <typename T> using Matrix = vector<vector<T> > ;

vector<pair<int, int> > GenerateGrid(int H, int W){
#define GetID(h, w) ((h) * W + (w))
  
  vector<pair<int, int> > es;
  for (int h = 0; h < H; h++){
    for (int w = 0; w < W; w++){
      if (h + 1 < H){
        es.emplace_back(GetID(h, w), GetID(h + 1, w));
        es.emplace_back(GetID(h + 1, w), GetID(h, w));
      }

      if (w + 1 < W){
        es.emplace_back(GetID(h, w), GetID(h, w + 1));
        es.emplace_back(GetID(h, w + 1), GetID(h, w));
      }
    }
  }
  return es;
}


template <typename S, typename T> void CheckAccuracy(int V, S *a, T *b, double tol){
  for (int v = 0; v < V; v++){
    double value_1 = a->QueryCentrality(v);
    double value_2 = b->QueryCentrality(v);
    EXPECT_NEAR(value_1, value_2, tol * V * V) << v << " " << V << " " << value_1 << " " << value_2 << endl;
  }
}


void CheckStatic(int V, const vector<pair<int, int> > &es){
  auto *a = new CentralityBrandes();
  auto *b = new DynamicCentralityBMS();
  a->PreCompute(es);
  b->PreCompute(es, 10000);
  CheckAccuracy(V, a, b, 0.02);
  delete a;
  delete b;
}

TEST(BMS_ON_GRID_SMALL0, ACCURACY){
  auto es = GenerateGrid(2, 3);
  CheckStatic(2 * 3, es);
}

TEST(BMS_ON_GRID_SMALL1, ACCURACY){
  auto es = GenerateGrid(5, 5);
  CheckStatic(5 * 5, es);
}

TEST(BMS_ON_GRID_SMALL2, ACCURACY){
  auto es = GenerateGrid(2, 10);
  CheckStatic(2 * 10, es);
}

TEST(BMS_ON_HAND_SMALL0, INSERT){
  size_t V = 5;
  vector<pair<int, int> > all_es;
  all_es.emplace_back(3, 4);
  all_es.emplace_back(2, 3);
  all_es.emplace_back(1, 2);
  all_es.emplace_back(1, 0);  
  all_es.emplace_back(0, 1);
  vector<pair<int, int> > es;
  for (size_t v = 0; v < V; v++){
    es.emplace_back(v, v);
  }
         
  auto *a = new CentralityBrandes();
  auto *b = new DynamicCentralityBMS();

  b->PreCompute(es, 10000);
  for (const auto &e : all_es){
    es.push_back(e);
    a->PreCompute(es);
    b->InsertEdge(e.fst, e.snd);
    CheckAccuracy(V, a, b, 0.03);
  }
  delete a;
  delete b;
}


TEST(BMS_ON_GRID_SMALL0, INSERT){
  size_t H = 7;
  size_t W = 7;
  size_t V = H * W;
  vector<pair<int, int> > es = GenerateGrid(H, W);
  vector<pair<int, int> > start_es;
  for (size_t v = 0; v < V; v++){
    start_es.emplace_back(v, v);
  }
  std::random_shuffle(es.begin(), es.end());
  
  for (int batch_size = 1; batch_size < 10; batch_size++){
    auto *a = new CentralityBrandes();
    auto *b = new DynamicCentralityBMS();
    a->PreCompute(start_es);
    b->PreCompute(start_es, 4000);
    
    vector<pair<int, int> > tmp_es = start_es;
    for (size_t i = 0; i < es.size(); i += batch_size){
      vector<pair<int, int> > add_es;
      for (size_t j = i; j < min(es.size(), i + batch_size); j++){
        tmp_es.push_back(es[j]);
        add_es.push_back(es[j]);
      }
      
      a->PreCompute(tmp_es);
      b->BatchInsert(add_es);
      CheckAccuracy(V, a, b, 0.03);
    }
    delete a;
    delete b;
  }
}
