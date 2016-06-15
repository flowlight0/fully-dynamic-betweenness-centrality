#include <algorithm>
#include "gtest/gtest.h"
#include "algorithm/centrality_base.hpp"
#include "algorithm/centrality_brandes.hpp"
#include "algorithm/centrality_sampling.hpp"
using namespace std;

class BaseGraphTest : public ::testing::Test {
protected:
  vector<pair<int, int> > es;
  vector<double> centrality_values;
  virtual void SetUp() = 0;
  virtual void TearDown(){};

  template <class Centrality>  void Check(double tolerance, int num_samples = -1){
    Centrality bc;
    bc.PreCompute(es, num_samples);
    for (int v = 0; v < int(centrality_values.size()); v++){
      ASSERT_NEAR(bc.QueryCentrality(v), centrality_values[v], tolerance) << es << " " << v << endl;
    }
  }
};

class SMALL0Test : public BaseGraphTest {
  virtual void SetUp() {
    es.emplace_back(0, 1);
    es.emplace_back(1, 0);
    es.emplace_back(1, 2);
    es.emplace_back(2, 3);
    es.emplace_back(3, 4);
    centrality_values = {0, 3, 4, 3, 0};
  }
};

class SMALL1Test : public BaseGraphTest {
  virtual void SetUp() {
    es.emplace_back(0, 1);
    es.emplace_back(1, 0);
    es.emplace_back(1, 2);
    es.emplace_back(2, 3);
    es.emplace_back(4, 5);
    centrality_values = {0, 2, 2, 0, 0, 0};
  }
};

template <int num_vs, bool has_direction> class LineGraphTest : public BaseGraphTest {
protected:
  virtual void SetUp() {
    for (int v = 0; v + 1 < num_vs; v++){
      es.emplace_back(v, v + 1);
      if (!has_direction) es.emplace_back(v + 1, v);
    }
    
    centrality_values.resize(num_vs);
    for (int v = 0; v < num_vs; v++){
      centrality_values[v] = v * (num_vs - v - 1) * (has_direction ? 1 : 2);
    }
  }
};

template <int height, int width, bool has_direction> class GridGraphTest : public BaseGraphTest {
  double ComputeNumPaths(int sh, int sw, int th, int tw){
    int dh = abs(sh - th);
    int dw = abs(sw - tw);
    
    double res = 1;
    for (int i = 1; i <= dh; i++){
      res = res * (dh + dw - i + 1) / i;
    }
    return res;
  }
  
  double ExactCentlarity(int h, int w){
    int num_vs = height * width;
    double sum = 0;
    for (int s = 0; s < num_vs; s++){
      for (int t = 0; t < num_vs; t++){
        int sh = s / width, sw = s % width;
        int th = t / width, tw = t % width;
        if (((sh <= th && sw <= tw) || !has_direction) &&
            abs(sh - h) + abs(th - h) == abs(sh - th) &&
            abs(sw - w) + abs(tw - w) == abs(sw - tw) &&
            (h != sh || w != sw) && (h != th || w != tw)){
          double num = ComputeNumPaths(sh, sw, h, w) * ComputeNumPaths(h, w, th, tw);
          double den = ComputeNumPaths(sh, sw, th, tw);
          sum += num / den;
        }
      }
    }
    return sum;
  }
  
protected:
  virtual void SetUp() {
    for (int h = 0; h < height; h++){
      for (int w = 0; w < width; w++){
        for (int i = 0; i < 2; i++){
          int nh = h + i;
          int nw = w + 1 - i;
          if (nw == width || nh ==  height) continue;
          int s = h  * width + w;
          int t = nh * width + nw;
          assert(s != t);
          es.emplace_back(s, t);
          if (!has_direction) es.emplace_back(t, s);
        }
      }
    }
    std::random_shuffle(es.begin(), es.end());
    centrality_values.resize(height * width);
    for (int h = 0; h < height; h++){
      for (int w = 0; w < width; w++){
        centrality_values[h * width + w] = ExactCentlarity(h, w);
      }
    }
  }
};


TEST_F(SMALL0Test, EXACT){
  Check<betweenness_centrality::CentralityBrandes>(1e-6);
}
TEST_F(SMALL0Test, APPROX){
  int num_vs = centrality_values.size();
  Check<betweenness_centrality::CentralitySampling>(1e-3 * num_vs * num_vs, 100000);
}

TEST(BETWEENNESS_ON_SMALL0, EXACT_RENAME){
  const double eps = 1e-6;
  vector<pair<int, int> > es;
  es.push_back(make_pair(0, 100));
  es.push_back(make_pair(100, 0));
  es.push_back(make_pair(100, 200));
  es.push_back(make_pair(200, 300));
  es.push_back(make_pair(300, 400));
  betweenness_centrality::CentralityBrandes bcn;
  bcn.PreCompute(es);
  ASSERT_NEAR((double)bcn.QueryCentrality(000) / 25, 0.00, eps);
  ASSERT_NEAR((double)bcn.QueryCentrality(100) / 25, 0.12, eps);
  ASSERT_NEAR((double)bcn.QueryCentrality(200) / 25, 0.16, eps);
  ASSERT_NEAR((double)bcn.QueryCentrality(300) / 25, 0.12, eps);
  ASSERT_NEAR((double)bcn.QueryCentrality(400) / 25, 0.00, eps);
  ASSERT_NEAR((double)bcn.QueryCentrality(1  ) / 25, 0.00, eps);
}

TEST_F(SMALL1Test, EXACT){
  Check<betweenness_centrality::CentralityBrandes>(1e-6);
}
TEST_F(SMALL1Test, APPROX){
  int num_vs = centrality_values.size();
  Check<betweenness_centrality::CentralitySampling>(1e-3 * num_vs * num_vs, 100000);
}


TEST(BETWEENNESS_ON_EMPTY, EXACT){
  const double eps = 1e-6;
  vector<pair<int, int> > es;
  betweenness_centrality::CentralityBrandes bcn;
  bcn.PreCompute(es);
  ASSERT_NEAR((double)bcn.QueryCentrality(0), 0.00, eps);
}

typedef LineGraphTest<50, true> BETWEENNESS_ON_DIRECTED_LINE;
TEST_F(BETWEENNESS_ON_DIRECTED_LINE, EXACT){
  Check<betweenness_centrality::CentralityBrandes>(1e-6);
}

TEST_F(BETWEENNESS_ON_DIRECTED_LINE, APPROX){
  int num_vs = centrality_values.size();
  Check<betweenness_centrality::CentralitySampling>(3e-2 * num_vs * num_vs, 5000);
}

typedef LineGraphTest<50, false> BETWEENNESS_ON_UNDIRECTED_LINE;
TEST_F(BETWEENNESS_ON_UNDIRECTED_LINE, EXACT){
  Check<betweenness_centrality::CentralityBrandes>(1e-6);
}

TEST_F(BETWEENNESS_ON_UNDIRECTED_LINE, APPROX){
  int num_vs = centrality_values.size();
  Check<betweenness_centrality::CentralitySampling>(3e-2 * num_vs * num_vs, 5000);
}

typedef GridGraphTest<3, 3, true > BETWEENNESS_ON_DIRECTED_GRID_TINY;
TEST_F(BETWEENNESS_ON_DIRECTED_GRID_TINY, EXACT){
  Check<betweenness_centrality::CentralityBrandes>(1e-6);
}
TEST_F(BETWEENNESS_ON_DIRECTED_GRID_TINY, APPROX){
  int num_vs = centrality_values.size();
  Check<betweenness_centrality::CentralitySampling>(3e-2 * num_vs * num_vs, 5000);
}

typedef GridGraphTest<3, 3, true > BETWEENNESS_ON_UNDIRECTED_GRID_TINY;
TEST_F(BETWEENNESS_ON_UNDIRECTED_GRID_TINY, EXACT){
  Check<betweenness_centrality::CentralityBrandes>(1e-6);
}
TEST_F(BETWEENNESS_ON_UNDIRECTED_GRID_TINY, APPROX){
  int num_vs = centrality_values.size();
  Check<betweenness_centrality::CentralitySampling>(3e-2 * num_vs * num_vs, 5000);
}


typedef GridGraphTest<10, 10, true > BETWEENNESS_ON_DIRECTED_GRID;
TEST_F(BETWEENNESS_ON_DIRECTED_GRID, EXACT){
  Check<betweenness_centrality::CentralityBrandes>(1e-6);
}
TEST_F(BETWEENNESS_ON_DIRECTED_GRID, APPROX){
  int num_vs = centrality_values.size();
  Check<betweenness_centrality::CentralitySampling>(3e-2 * num_vs * num_vs, 5000);
}

typedef GridGraphTest<10, 10, false> BETWEENNESS_ON_UNDIRECTED_GRID;
TEST_F(BETWEENNESS_ON_UNDIRECTED_GRID, EXACT){
  Check<betweenness_centrality::CentralityBrandes>(1e-6);
}
TEST_F(BETWEENNESS_ON_UNDIRECTED_GRID, APPROX){
  int num_vs = centrality_values.size();
  Check<betweenness_centrality::CentralitySampling>(3e-2 * num_vs * num_vs, 5000);
}

