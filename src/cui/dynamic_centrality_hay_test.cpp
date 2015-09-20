#include "algorithm/dynamic_centrality_hay.hpp"
#include "algorithm/dynamic_centrality_naive.hpp"
#include "gtest/gtest.h"
using namespace betweenness_centrality;
using namespace std;

namespace  {
  vector<double> GetCentralityArray(CentralityBase *a, int V){
    vector<double> ans(V);
    for (int v = 0; v < V; v++){
      ans[v] = a->QueryCentrality(v);
    }
    return ans;
  }
} /*  */



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

 // void test_update(DynamicCentralityBase *a, DynamicCentralityBase *b, const vector<pair<int, int> > &es){
  
//   vector<vector<int> > G(adjacencyLists(es));
//   int V = G.size();
//   const double tolerance = 1e-5;
  
  
//   a->PreCompute(es, -1);
//   b->PreCompute(es, -1);
//   CheckError(a, b, V, tolerance);
    
//   vector<int> queries = GenerateRandomQueries(min((int)es.size() / 2, 30), es);    
//   vector<int> deletions = queries;
//   vector<int> insertions= queries; reverse(insertions.begin(), insertions.end());
//   vector<int> is_deleted(es.size());
    
//   for (int e : deletions){
//     a->DeleteEdge(es[e].first, es[e].second);
//     b->DeleteEdge(es[e].first, es[e].second);

//     vector<double> value_a;
//     vector<double> value_b;
//     for (int v = 0; v < V; v++) value_a.push_back(a->QueryCentrality(v));
//     for (int v = 0; v < V; v++) value_b.push_back(b->QueryCentrality(v));

    
//     for (int v = 0; v < V; v++){
//       ASSERT_NEAR(value_a[v], value_b[v], tolerance)
//         << v << " " << V << endl
//         << "ans: " << value_a << endl
//         << "out: " << value_b << endl;
//     }
//     is_deleted[e] = true;
//   }
    
//   vector<pair<int, int> > es_;

//   for (size_t e = 0; e < es.size(); e++){
//     if (!is_deleted[e]) es_.push_back(es[e]);
//   }

//   // cout << adjacencyLists(es_) << endl;
//   for (int e : insertions){
//     a->InsertEdge(es[e].first, es[e].second);
//     b->InsertEdge(es[e].first, es[e].second);
//     es_.push_back(es[e]);
      
//     vector<double> value_a(GetCentralityArray(a, V));
//     vector<double> value_b(GetCentralityArray(b, V));
    
//     for (int v = 0; v < V; v++){
//       ASSERT_NEAR(value_a[v], value_b[v], tolerance)
//         << v << " " << V << endl
//         << "ans: " << value_a << endl
//         << "out: " << value_b << endl;
//     }
//   }
// }

// void test_update_on_random_graph(int V, int num_graphs, double prob){
//   srand(0);
//   unsigned int seed = 0;

//   Interface 
//     *a = dynamic_centrality::get_algorithm_from_name("naive"),
//     *b = dynamic_centrality::get_algorithm_from_name("dynamic-sketch");

//   while (num_graphs--){
//     vector<pair<int, int> > es(random_graph(V, prob, &seed));
//     test_update(a, b, es);
//   }
// }

vector<pair<int, int> > generate_grid(int H, int W){
#define getID(h, w) ((h) * W + (w))

  vector<pair<int, int> > es;
  for (int h = 0; h < H; h++) {
    for (int w = 0; w < W; w++){
      if (h + 1 < H){
        es.push_back(make_pair(getID(h, w), getID(h + 1, w)));
        es.push_back(make_pair(getID(h + 1, w), getID(h, w)));
      }
      if (w + 1 < W){
        es.push_back(make_pair(getID(h, w), getID(h, w + 1)));
        es.push_back(make_pair(getID(h, w + 1), getID(h, w)));
      }
    }
  }
  return es;
}

// void test_update_on_grid_graph(int H, int W){
//   vector<pair<int, int> > es(generate_grid(H, W));
//   // DynamicCentralityBase *a = 
//   Interface 
//     *a = dynamic_centrality::get_algorithm_from_name("naive"),
//     *b = dynamic_centrality::get_algorithm_from_name("dynamic-sketch");
//   test_update(a, b, es);
// }

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

// TEST(FAST_SKETCH_UPDATE, TINY_RANDOM1){ test_update_on_random_graph(5, 10, 0.1); }
// TEST(FAST_SKETCH_UPDATE, TINY_RANDOM15){ test_update_on_random_graph(5, 10, 0.15); }
// TEST(FAST_SKETCH_UPDATE, TINY_RANDOM3){ test_update_on_random_graph(4, 10, 0.3); }
// TEST(FAST_SKETCH_UPDATE, TINY_RANDOM5){ test_update_on_random_graph(4, 10, 0.5); }
// TEST(FAST_SKETCH_UPDATE, TINY_RANDOM8){ test_update_on_random_graph(4, 10, 0.8); }
// TEST(FAST_SKETCH_UPDATE, TINY_RANDOM10){test_update_on_random_graph(5, 10, 1.0); }

// TEST(FAST_SKETCH_UPDATE, TINY_GRID1){ test_update_on_grid_graph(2, 4); }
// TEST(FAST_SKETCH_UPDATE, TINY_GRID2){ test_update_on_grid_graph(3, 3); }

// TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM1){ test_update_on_random_graph(10, 10, 0.1); }
// TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM3){ test_update_on_random_graph(10, 10, 0.3); }
// TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM5){ test_update_on_random_graph(10, 10, 0.5); }
// TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM8){ test_update_on_random_graph(10, 10, 0.8); }
// TEST(FAST_SKETCH_UPDATE, SMALL_RANDOM10){test_update_on_random_graph(10, 10, 1.0); }

// TEST(FAST_SKETCH_UPDATE, SMALL_GRID1){ test_update_on_grid_graph(4, 4); }
// TEST(FAST_SKETCH_UPDATE, SMALL_GRID2){ test_update_on_grid_graph(5, 3); }

// TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM1){ test_update_on_random_graph(30, 5, 0.1); }
// TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM3){ test_update_on_random_graph(30, 5, 0.3); }
// TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM5){ test_update_on_random_graph(30, 5, 0.5); }
// TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM8){ test_update_on_random_graph(30, 5, 0.8); }
// TEST(FAST_SKETCH_UPDATE, MIDDLE_RANDOM10){test_update_on_random_graph(30, 5, 1.0); }

// TEST(FAST_SKETCH_UPDATE, MIDDLE_GRID1){ test_update_on_grid_graph(5, 10); }
// TEST(FAST_SKETCH_UPDATE, MIDDLE_GRID2){ test_update_on_grid_graph(7, 7); }

// void test_different_ball_size_on_grid_graph(int H, int W){
//   srand(0);
//   vector<pair<int, int> > es(generate_grid(H, W));
//   DynamicSketch
//     *a = new DynamicSketch(),
//     *b = new DynamicSketch();
//   for (int x = 0; x < 10; x++){
//     b->setQueryBallSize(x);
//     test_update(a, b, es);
//   }
//   delete a;
//   delete b;
// }

// TEST(FAST_SKETCH_BALL_SIZE, TINY_GRID1){ test_different_ball_size_on_grid_graph(2, 4); }
// TEST(FAST_SKETCH_BALL_SIZE, TINY_GRID2){ test_different_ball_size_on_grid_graph(3, 3); }
// TEST(FAST_SKETCH_BALL_SIZE, SMALL_GRID1){ test_different_ball_size_on_grid_graph(4, 4); }
// TEST(FAST_SKETCH_BALL_SIZE, SMALL_GRID2){ test_different_ball_size_on_grid_graph(5, 3); }
// TEST(FAST_SKETCH_BALL_SIZE, MIDDLE_GRID1){ test_different_ball_size_on_grid_graph(5, 10); }
// TEST(FAST_SKETCH_BALL_SIZE, MIDDLE_GRID2){ test_different_ball_size_on_grid_graph(7, 7); }
