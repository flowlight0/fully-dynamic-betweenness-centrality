#include "algorithm/dynamic_betweenness_centrality_bergamini.hpp"
#include "common.hpp"
#include "gtest/gtest.h"
#include <memory>
using namespace betweenness_centrality;
using namespace std;


template <typename T> using Matrix = vector<vector<T> > ;

vector<pair<int, int> > GenerateGrid(int H, int W){
#define getID(h, w) ((h) * W + (w))
  
  vector<pair<int, int> > es;
  for (int h = 0; h < H; h++){
    for (int w = 0; w < W; w++){
      if (h + 1 < H){
        es.emplace_back(getID(h, w), getID(h + 1, w));
        es.emplace_back(getID(h + 1, w), getID(h, w));
      }

      if (w + 1 < W){
        es.emplace_back(getID(h, w), getID(h, w + 1));
        es.emplace_back(getID(h, w + 1), getID(h, w));
      }
    }
  }
  return es;
}


template <typename S, typename T> void CheckAccuracy(int V, S *a, T *b, double tol){
  for (int v = 0; v < V; v++){
    double value_1 = a->QueryCentrality(v);
    double value_2 = b->QueryCentrality(v);
    EXPECT_NEAR(value_1, value_2, tol * V * V) << v << " " << value_1 << " " << value_2 << endl;
  }
}


void CheckStatic(int V, const vector<pair<int, int> > &es){
  auto *a = new BetweennessCentralityNaive();
  auto *b = new DynamicBetweennessCentralityBergamini();
  a->PreCompute(es);
  b->PreCompute(es, 10000);
  CheckAccuracy(V, a, b, 0.02);
  delete a;
  delete b;
}

TEST(BERGAMINI_ON_GRID_SMALL0, ACCURACY){
  auto es = GenerateGrid(2, 3);
  CheckStatic(2 * 3, es);
}

TEST(BERGAMINI_ON_GRID_SMALL1, ACCURACY){
  auto es = GenerateGrid(5, 5);
  CheckStatic(5 * 5, es);
}

TEST(BERGAMINI_ON_GRID_SMALL2, ACCURACY){
  auto es = GenerateGrid(2, 10);
  CheckStatic(2 * 10, es);
}

// TEST(UPDATE, GRID_SMALL){
//   int H = 7;
//   int W = 7;
//   int V = H * W;
//   auto es    = GenerateGrid(H, W);
//   auto dummy = vector<pair<int, int> >(1, make_pair(V - 1, V  - 1));
//   std::random_shuffle(es.begin(), es.end());
  
//   for (int batch_size = 1; batch_size < 10; batch_size++){
//     auto *a = new BetweennessCentralityNaive();
//     auto *b = new DynamicBetweennessCentralityBergamini();
//     a->PreCompute(dummy);
//     b->PreCompute(dummy, 10000);
    
//     vector<pair<int, int> > tmp_es = dummy;
//     for (size_t i = 0; i < es.size(); i += batch_size){
//       vector<pair<int, int> > add_es;
//       for (size_t j = i; j < min(es.size(), i + batch_size); j++){
//         tmp_es.push_back(es[j]);
//         add_es.push_back(es[j]);
//       }
      
//       a->PreCompute(tmp_es);
//       b->BatchInsert(add_es);
//       CheckAccuracy(V, a, b, 0.03);
//     }
//     delete a;
//     delete b;
//   }
// }
