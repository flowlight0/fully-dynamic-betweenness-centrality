#include "algorithm/dynamic_centrality.hpp"
#include "algorithm/dynamic_centrality_hay.hpp"
#include "algorithm/dynamic_centrality_naive.hpp"
#include "gtest/gtest.h"
using namespace betweenness_centrality;
using namespace std;

inline void CheckError(CentralityBase *a, CentralityBase *b, int V, double error_threshold){
  for (int v = 0; v < V; v++){
    ASSERT_NEAR(a->QueryCentrality(v), b->QueryCentrality(v), error_threshold)
      << v << " " << V << endl;;
  }
}

vector<int> GenerateRandomQueries(int num_queries, const vector<pair<int, int> > &es){
  vector<int> queries;
  set<int>    in_queries;
  for (int i = 0; i < num_queries; i++){
    int e = rand() % es.size();
    while (in_queries.count(e)) e = rand() % es.size();
    queries.push_back(e);
    in_queries.insert(e);
  }
  return queries;
}

void TestUpdate(DynamicCentralityBase *a, DynamicCentralityBase *b, const vector<pair<int, int> > &es){
   
  // vector<vector<int> > G(adjacencyLists(es));
  int V = 0;
  for (const auto &e : es){
    V = max({V, e.fst + 1, e.snd + 1});
  }
  vector<vector<int> > G(V);
  for (const auto &e : es){
    G[e.fst].push_back(e.snd);
  }
   
  const double tolerance = 1e-5;
  a->PreCompute(es, -1);
  b->PreCompute(es, -1);
  CheckError(a, b, V, tolerance);
    
  vector<int> queries = GenerateRandomQueries(min((int)es.size() / 2, 30), es);    
  vector<int> deletions = queries;
  vector<int> insertions= queries; reverse(insertions.begin(), insertions.end());
  vector<int> is_deleted(es.size());
    
  for (int e : deletions){
    a->DeleteEdge(es[e].first, es[e].second);
    b->DeleteEdge(es[e].first, es[e].second);
    CheckError(a, b, V, tolerance);
    is_deleted[e] = true;
  }
    
  vector<pair<int, int> > es_;

  for (size_t e = 0; e < es.size(); e++){
    if (!is_deleted[e]) es_.push_back(es[e]);
  }

  for (int e : insertions){
    a->InsertEdge(es[e].first, es[e].second);
    b->InsertEdge(es[e].first, es[e].second);
    CheckError(a, b, V, tolerance);
    es_.push_back(es[e]);
  }
}

vector<pair<int, int> > RandomGraph(int V, double e_prob, unsigned *seed)
{
  vector<pair<int, int> > es;
  for (int i = 0; i < V; i++){
    for (int j = 0; j < V; j++){
      double p = (double)rand_r(seed) / RAND_MAX;
      if (i != j && p < e_prob) es.emplace_back(i, j);
    }
  }
  return es;
}

void TestUpdateOnRandomGraph(int V, int num_graphs, double prob){
  srand(0);
  unsigned int seed = 0;
  DynamicCentralityNaive dcn;
  DynamicCentralityHAY dch;
  while (num_graphs--){
    vector<pair<int, int> > es(RandomGraph(V, prob, &seed));
    cout << es << endl;
    TestUpdate(&dcn, &dch, es);
  }
}

vector<pair<int, int> > GenerateGrid(int H, int W){
#define GetID(h, w) ((h) * W + (w))

  vector<pair<int, int> > es;
  for (int h = 0; h < H; h++) {
    for (int w = 0; w < W; w++){
      if (h + 1 < H){
        es.push_back(make_pair(GetID(h, w), GetID(h + 1, w)));
        es.push_back(make_pair(GetID(h + 1, w), GetID(h, w)));
      }
      if (w + 1 < W){
        es.push_back(make_pair(GetID(h, w), GetID(h, w + 1)));
        es.push_back(make_pair(GetID(h, w + 1), GetID(h, w)));
      }
    }
  }
  return es;
}

void TestUpdateOnGridGraph(int H, int W){
  vector<pair<int, int> > es(GenerateGrid(H, W));
  DynamicCentralityNaive dcn;
  DynamicCentralityHAY dch;
  TestUpdate(&dcn, &dch, es);
}

TEST(FAST_SKETCH_UPDATE, TINY_BALL_DELETE){
  srand(0);
  vector<pair<int, int> > es;
  int V = 7;
  es.push_back(make_pair(0, 1));
  es.push_back(make_pair(1, 2));
  es.push_back(make_pair(2, 3));
  es.push_back(make_pair(3, 4));
  es.push_back(make_pair(0, 6));
  es.push_back(make_pair(1, 5));
  es.push_back(make_pair(5, 6));
  
  DynamicCentralityNaive dcn;
  DynamicCentralityHAY dch;
  
  dcn.PreCompute(es, -1);
  dch.PreCompute(es, -1);

  CheckError(&dcn, &dch, V, 1e-7);
  dcn.DeleteEdge(0, 6); dch.DeleteEdge(0, 6);
  CheckError(&dcn, &dch, V, 1e-7);
  dcn.InsertEdge(6, 4); dch.InsertEdge(6, 4);
  CheckError(&dcn, &dch, V, 1e-7);
}

TEST(FAST_SKETCH_UPDATE, TINY_RANDOM1){ TestUpdateOnRandomGraph(5, 10, 0.1); }
TEST(FAST_SKETCH_UPDATE, TINY_RANDOM15){ TestUpdateOnRandomGraph(5, 10, 0.15); }
TEST(FAST_SKETCH_UPDATE, TINY_RANDOM3){ TestUpdateOnRandomGraph(4, 10, 0.3); }
TEST(FAST_SKETCH_UPDATE, TINY_RANDOM5){ TestUpdateOnRandomGraph(4, 10, 0.5); }
TEST(FAST_SKETCH_UPDATE, TINY_RANDOM8){ TestUpdateOnRandomGraph(4, 10, 0.8); }
TEST(FAST_SKETCH_UPDATE, TINY_RANDOM10){TestUpdateOnRandomGraph(5, 10, 1.0); }

TEST(FAST_SKETCH_UPDATE, TINY_GRID1){ TestUpdateOnGridGraph(2, 4); }
TEST(FAST_SKETCH_UPDATE, TINY_GRID2){ TestUpdateOnGridGraph(3, 3); }

TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM1){ TestUpdateOnRandomGraph(10, 10, 0.1); }
TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM3){ TestUpdateOnRandomGraph(10, 10, 0.3); }
TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM5){ TestUpdateOnRandomGraph(10, 10, 0.5); }
TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM8){ TestUpdateOnRandomGraph(10, 10, 0.8); }
TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM10){TestUpdateOnRandomGraph(10, 10, 1.0); }

TEST(FAST_SKETCH_UPDATE, SMALL_GRID1){ TestUpdateOnGridGraph(4, 4); }
TEST(FAST_SKETCH_UPDATE, SMALL_GRID2){ TestUpdateOnGridGraph(5, 3); }

TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM1){ TestUpdateOnRandomGraph(30, 5, 0.1); }
TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM3){ TestUpdateOnRandomGraph(30, 5, 0.3); }
TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM5){ TestUpdateOnRandomGraph(30, 5, 0.5); }
TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM8){ TestUpdateOnRandomGraph(30, 5, 0.8); }
TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM10){TestUpdateOnRandomGraph(30, 5, 1.0); }

TEST(FAST_SKETCH_UPDATE, MIDDLE_GRID1){ TestUpdateOnGridGraph(5, 10); }
TEST(FAST_SKETCH_UPDATE, MIDDLE_GRID2){ TestUpdateOnGridGraph(7, 7); }

void TestVariousBallSize(int H, int W){
  srand(0);
  vector<pair<int, int> > es(GenerateGrid(H, W));
  DynamicCentralityNaive dcn;
  DynamicCentralityHAY dch;
  
  for (int x = 0; x < 10; x++){
    dch.SetTradeOffParam(x);
    TestUpdate(&dcn, &dch, es);
  }
}

TEST(FAST_SKETCH_BALL_SIZE, TINY_GRID1){ TestVariousBallSize(2, 4); }
TEST(FAST_SKETCH_BALL_SIZE, TINY_GRID2){ TestVariousBallSize(3, 3); }
TEST(FAST_SKETCH_BALL_SIZE, SMALL_GRID1){ TestVariousBallSize(4, 4); }
TEST(FAST_SKETCH_BALL_SIZE, SMALL_GRID2){ TestVariousBallSize(5, 3); }
TEST(FAST_SKETCH_BALL_SIZE, MIDDLE_GRID1){ TestVariousBallSize(5, 10); }
TEST(FAST_SKETCH_BALL_SIZE, MIDDLE_GRID2){ TestVariousBallSize(7, 7); }
