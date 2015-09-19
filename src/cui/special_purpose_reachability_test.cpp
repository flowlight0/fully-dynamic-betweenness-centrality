#include "algorithm/special_purpose_reachability_index.hpp"
#include "common.hpp"
#include "gtest/gtest.h"
using namespace betweenness_centrality::special_purpose_reachability_index;
using namespace std;

template <typename T> using Matrix = vector<vector<T> > ;
typedef SpecialPurposeReachabilityIndex SPRIndex;

void Check(const Matrix<int> &adj_matrix, const pair<SPRIndex*, Matrix<ReachabilityQuerier*> > &p){
  int n = adj_matrix.size();
  Matrix<int> reach = adj_matrix;

  for (int k = 0; k < n; k++){
    for (int i = 0; i < n; i++){
      for (int j = 0; j < n; j++){
        if (reach[i][k] & reach[k][j]) reach[i][j] = true;
      }
    }
  }

  for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      if (i != j){
        ASSERT_EQ(reach[i][j], p.second[i][j]->Reach()) 
          << "source: " << i << "\t" << "target: " << j << "\t"
          << "roots:  " << p.first->GetRoots() << "\t"
          << "reach:  " << p.second[i][j]->GetIndexNodes() << endl
          << "trees:  " << p.first->GetTrees() << endl
          << "graph:  " << adj_matrix << endl;
      }
    }
  }
}

const int tiny_num_nodes = 5;
const int tiny_num_graph = 10;
const int small_num_nodes = 10;
const int small_num_graph = 10;
const int middle_num_nodes = 30;
const int middle_num_graph = 3;


Matrix<int> GenerateERGraph(int n, double thres){
  Matrix<int> adj_matrix(n, vector<int>(n, 0));
  for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      if (i != j && (double)rand() / RAND_MAX < thres) adj_matrix[i][j] = 1;
    }
  }
  return adj_matrix;
}

pair<Matrix<int>, Matrix<int> > BuildAdj(const Matrix<int> &adj_mat){
  int n = adj_mat.size();
  Matrix<int> fg(n), bg(n);

  for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      if (i != j && adj_mat[i][j]){
        fg[i].push_back(j);
        bg[j].push_back(i);
      }
    }
  }
  return make_pair(fg, bg);
}

pair<SPRIndex*, Matrix<ReachabilityQuerier*> > BuildIndex(Matrix<int> &fg, Matrix<int> &bg, int num_trees){
  int n = fg.size();
  SPRIndex *pr_index = new SPRIndex(&fg, &bg, num_trees);
  Matrix<ReachabilityQuerier*> prqs(n, vector<ReachabilityQuerier*>(n, nullptr));

  for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      if (i != j){
        prqs[i][j] = pr_index->CreateQuerier(i, j);
      }
    }
  }
  return make_pair(pr_index, prqs);
}


void StaticTest(int n, int q, double thres, int num_trees){
  srand(0);
  while(q--){
    Matrix<int> adj_matrix(GenerateERGraph(n, thres));
    auto adj_list = BuildAdj(adj_matrix);
    auto index = BuildIndex(adj_list.first, adj_list.second, num_trees);
    Check(adj_matrix, index);
    delete index.first;
  }
}

void InsertEdge(pair<vector<vector<int> >, vector<vector<int> > > &adj_lists,
                Matrix<int> &adj_matrix,
                SPRIndex *pivot_index,
                int u,
                int v)
{
  vector<vector<int> > &fadj = adj_lists.first;
  vector<vector<int> > &badj = adj_lists.second;
  if (adj_matrix[u][v] == 0){
    adj_matrix[u][v] = 1;
    fadj[u].push_back(v);
    badj[v].push_back(u);
    pivot_index->InsertEdge(u, v);
  }
}

// void deleteEdge(pair<vector<vector<int> >, vector<vector<int> > > &adj_lists,
//                  Matrix<int> &adj_matrix,
//                  SPRIndex *pivot_index,
//                 int u,
//                 int v)
// {
//   vector<vector<int> > &fadj = adj_lists.first;
//   vector<vector<int> > &badj = adj_lists.second;
//   if (adj_matrix[u][v] == 1){
//     adj_matrix[u][v] = 0;
//     fadj[u].erase(remove(ALL(fadj[u]), v), fadj[u].end());
//     badj[v].erase(remove(ALL(badj[v]), u), badj[v].end());
//     pivot_index->deleteEdge(u, v);
//   }
// }

// void InsertNode(pair<vector<vector<int> >, vector<vector<int> > > &adj_lists,
//                 Matrix<int> &adj_matrix,
//                 SPRIndex *pivot_index,
//                 Matrix<ReachabilityQuerier*> *pivot_matrix,
//                 int u)
// {
//   vector<vector<int> > &fadj = adj_lists.first;
//   vector<vector<int> > &badj = adj_lists.second;

//   int num_nodes = fadj.size();
  
//   if (u >= num_nodes){
//     num_nodes = u + 1;
    
//     adj_matrix.resize(num_nodes);
//     REP(v, num_nodes) adj_matrix[v].resize(num_nodes, 0);

//     fadj.resize(num_nodes);
//     badj.resize(num_nodes);

//     pivot_matrix->resize(num_nodes);
//     REP(v, num_nodes){
//       pivot_matrix->at(v).resize(num_nodes, nullptr);
//     }

//     pivot_index->InsertNode(u);
//     REP (u, num_nodes) REP(v, num_nodes){
//       if (pivot_matrix->at(u)[v] == nullptr && u != v){
//         pivot_matrix->at(u)[v] = pivot_index->createQuerier(u, v);
//       }
//     }
//   } else {
//     assert(fadj[u].empty() && badj[u].empty());
//     pivot_index->InsertNode(u);
//   }
// }

// void DeleteNode(pair<vector<vector<int> >, vector<vector<int> > > &adj_lists,
//                 Matrix<int> &adj_matrix,
//                 SPRIndex *pivot_index,
//                 int u)
// {
  
//   vector<vector<int> > &fadj = adj_lists.first;
//   vector<vector<int> > &badj = adj_lists.second;
//   int num_nodes = fadj.size();
//   assert(0 <= u && u < num_nodes);
  
//   vector<int> fvs;
//   vector<int> bvs;
//     REP(v, num_nodes) REP(w, num_nodes){
//     if (v == u && adj_matrix[v][w]) fvs.push_back(w);
//     if (w == u && adj_matrix[v][w]) bvs.push_back(v);
//     if (v == u || w == u ) adj_matrix[v][w] = false;
//     assert(v != w || !adj_matrix[v][w] );
//   }
//   fadj[u].clear();
//   badj[u].clear();
//   // cout << fadj << " " << fvs << " " << u << endl;
//   for (int v : fvs) {
//     assert(v != u);
//     auto iter = find(ALL(badj[v]), u);
//     assert(iter != badj[v].end());
//     badj[v].erase(iter);
//     assert(find(ALL(badj[v]), u) == badj[v].end());
//   }

//   for (int v : bvs) {
//     assert(v != u);
//     auto iter = find(ALL(fadj[v]), u);
//     assert(iter != fadj[v].end());
//     fadj[v].erase(iter);
//     assert(find(ALL(fadj[v]), u) == fadj[v].end());
//   }
//   pivot_index->DeleteNode(u, fvs, bvs);
// }

void EdgeInsertTest(int n, int q, int num_trees){
  srand(0);
  while(q--){
    Matrix<int> adj_matrix(GenerateERGraph(n, 0.1));
    auto adj_lists = BuildAdj(adj_matrix);
    auto p = BuildIndex(adj_lists.first, adj_lists.second, num_trees);
    int num_edges = 0;

    while (num_edges++ * 2 < n * n){
      int s = rand() % n, t = rand() % n;
      while (adj_matrix[s][t]) s = rand () % n, t = rand() % n;
      
      InsertEdge(adj_lists, adj_matrix, p.first, s, t);
      Check(adj_matrix, p);
    }
    delete p.first;
  }
}

// void EdgeDeleteTest(int n, int q, int num_trees){
//   srand(0);
  
//   while(q--){
//     Matrix<int> adj_matrix(GenerateERGraph(n, -1));
//     REP(i, n) REP(j, n) if (i != j){
//       adj_matrix[i][j] = 1;
//     }
//     auto adj_lists = BuildAdj(adj_matrix);
//     auto p = BuildIndex(adj_lists.first, adj_lists.second, num_trees);
    
//     for (int num_edges = n * (n - 1); num_edges > n * n * 0.1; num_edges--){
//       int s = rand() % n, t = rand() % n;
//       while (!adj_matrix[s][t]){ s = rand () % n, t = rand() % n; }
      
//       deleteEdge(adj_lists, adj_matrix, p.first, s, t);
//       Check(adj_matrix, p);
//     }
//     delete p.first;
//   }
// }

// void EdgeRandomTest(int n, int q, int num_trees){
//   srand(0);
//   while(q--){
//     Matrix<int> adj_matrix(GenerateERGraph(n, -1));
//     REP(i, n) REP(j, n) if (i != j){
//       adj_matrix[i][j] = 1;
//     }
//     auto adj_lists = BuildAdj(adj_matrix);
//     auto p = BuildIndex(adj_lists.first, adj_lists.second, num_trees);
    
//     REP(c, 20){
//       int s = rand() % n, t = rand() % n;
//       while (s == t) {s = rand () % n, t = rand() % n; }

//       if (adj_matrix[s][t]){
//         deleteEdge(adj_lists, adj_matrix, p.first, s, t);
//       } else {
//         InsertEdge(adj_lists, adj_matrix, p.first, s, t);
//       }
//       Check(adj_matrix, p);
//     }
//     delete p.first;
//   }
// }


// void NodeInsertTest(int n, int q, int num_trees){
//   srand(0);
//   while (q--){
//     Matrix<int> adj_matrix(GenerateERGraph(n, -1));
//     auto adj_lists = BuildAdj(adj_matrix);
//     auto p = BuildIndex(adj_lists.first, adj_lists.second, num_trees);
//     REP(c, 20){
//       InsertNode(adj_lists, adj_matrix, p.first, &p.second, n + c);
//       InsertEdge(adj_lists, adj_matrix, p.first, n + c, 0);
//       InsertEdge(adj_lists, adj_matrix, p.first, 0, n + c);
//       Check(adj_matrix, p);
//     }
//     delete p.first;
//   }
// }

// void NodeDeleteTest(int n, int q, int num_trees){
//   srand(0);
//   while (q--){
//     Matrix<int> adj_matrix(GenerateERGraph(n, 0.5));
//     auto adj_lists = BuildAdj(adj_matrix);
//     auto p = BuildIndex(adj_lists.first, adj_lists.second, num_trees);

//     vector<int> active(n, true);

//     REP(c, n * 0.7){
//       int u = rand() % n;
//       while (!active[u]) u = rand() % n;
//       active[u] = false;
//       // DEBUG(active);
//       DeleteNode(adj_lists, adj_matrix, p.first, u);
//       Check(adj_matrix, p);
//     }
//   }
// }

// void node_delete_without_roots_test(int n, int q, int num_trees){
//   srand(0);
//   while (q--){
//     Matrix<int> adj_matrix(GenerateERGraph(n, 0.5));
//     auto adj_lists = BuildAdj(adj_matrix);
//     auto p = BuildIndex(adj_lists.first, adj_lists.second, num_trees);
    
//     vector<int> active(n, true);
//     vector<int> is_root(n);
//     for (int v : p.first->getRoots()){
//       is_root[v] = true;
//     }
    
//     REP(c, n * 0.7){
//       if (c + p.first->getRoots().size() == size_t(n)) break;
//       int u = rand() % n;
//       while (!active[u] || is_root[u]) u = rand() % n;
//       active[u] = false;
//       // cout << active << " " << "ACTIVE " << endl;
//       DeleteNode(adj_lists, adj_matrix, p.first, u);
//       Check(adj_matrix, p);
//     }
//     delete p.first;
//   }
// }


TEST(REACHABILITY_STATIC, WITHOUT_TREE_TINY020){ StaticTest(tiny_num_nodes, tiny_num_graph, 0.20, 0); }
TEST(REACHABILITY_STATIC, WITHOUT_TREE_TINY030){ StaticTest(tiny_num_nodes, tiny_num_graph, 0.30, 0); }
TEST(REACHABILITY_STATIC, WITHOUT_TREE_TINY050){ StaticTest(tiny_num_nodes, tiny_num_graph, 0.50, 0); }
TEST(REACHABILITY_STATIC, WITHOUT_TREE_SMALL015){ StaticTest(small_num_nodes, small_num_graph, 0.15, 0); }
TEST(REACHABILITY_STATIC, WITHOUT_TREE_SMALL020){ StaticTest(small_num_nodes, small_num_graph, 0.20, 0); }
TEST(REACHABILITY_STATIC, WITHOUT_TREE_SMALL030){ StaticTest(small_num_nodes, small_num_graph, 0.30, 0); }
TEST(REACHABILITY_STATIC, WITHOUT_TREE_SMALL050){ StaticTest(small_num_nodes, small_num_graph, 0.50, 0); }

// TEST(REACHABILITY_EDGE_INSERT, WITHOUT_TREE_TINY)  { EdgeInsertTest(tiny_num_nodes, tiny_num_graph, 0); }
// TEST(REACHABILITY_EDGE_INSERT, WITHOUT_TREE_SMALL) { EdgeInsertTest(small_num_nodes, small_num_graph, 0); }
// TEST(REACHABILITY_EDGE_DELETE, WITHOUT_TREE_TINY)  { EdgeDeleteTest(tiny_num_nodes, tiny_num_graph, 0); }
// TEST(REACHABILITY_EDGE_DELETE, WITHOUT_TREE_SMALL) { EdgeDeleteTest(small_num_nodes, small_num_graph, 0);}
// TEST(REACHABILITY_EDGE_RANDOM, WITHOUT_TREE_TINY){ EdgeRandomTest(tiny_num_nodes, tiny_num_graph, 0); }
// TEST(REACHABILITY_EDGE_RANDOM, WITHOUT_TREE_SMALL){ EdgeRandomTest(small_num_nodes, small_num_graph, 0); }

// TEST(REACHABILITY_NODE_INSERT, WITHOUT_TREE_TINY){ NodeInsertTest(tiny_num_nodes, tiny_num_graph, 0); }
// TEST(REACHABILITY_NODE_INSERT, WITHOUT_TREE_SMALL){ NodeInsertTest(small_num_nodes, small_num_graph, 0); }
// TEST(REACHABILITY_NODE_DELETE, WITHOUT_TREE_TINY){ NodeDeleteTest(tiny_num_nodes, tiny_num_graph, 0);}
// TEST(REACHABILITY_NODE_DELETE, WITHOUT_TREE_SMALL){ NodeDeleteTest(small_num_nodes, small_num_graph, 0); }
// ////////////////////////////////////////////////////////////////////////////////

TEST(REACHABILITY_STATIC, WITH_TREE_TINY020){ StaticTest(tiny_num_nodes, tiny_num_graph, 0.20, 5); }
TEST(REACHABILITY_STATIC, WITH_TREE_TINY030){ StaticTest(tiny_num_nodes, tiny_num_graph, 0.30, 5); }
TEST(REACHABILITY_STATIC, WITH_TREE_TINY050){ StaticTest(tiny_num_nodes, tiny_num_graph, 0.50, 5); }
TEST(REACHABILITY_STATIC, WITH_TREE_SMALL015){ StaticTest(small_num_nodes, small_num_graph, 0.15, 5);}
TEST(REACHABILITY_STATIC, WITH_TREE_SMALL020){ StaticTest(small_num_nodes, small_num_graph, 0.20, 5); }
TEST(REACHABILITY_STATIC, WITH_TREE_SMALL030){ StaticTest(small_num_nodes, small_num_graph, 0.30, 5); }
TEST(REACHABILITY_STATIC, WITH_TREE_SMALL050){ StaticTest(small_num_nodes, small_num_graph, 0.50, 5); }

// TEST(REACHABILITY_EDGE_INSERT, WITH_TREE_TINY){ EdgeInsertTest(tiny_num_nodes, tiny_num_graph, 1); }
// TEST(REACHABILITY_EDGE_INSERT, WITH_TREE_SMALL){ EdgeInsertTest(small_num_nodes, small_num_graph, 5); }
// TEST(REACHABILITY_EDGE_DELETE, WITH_TREE_TINY){ EdgeDeleteTest(tiny_num_nodes, tiny_num_graph, 1); }
// TEST(REACHABILITY_EDGE_DELETE, WITH_TREE_SMALL){ EdgeDeleteTest(small_num_nodes, small_num_graph, 5); }
// TEST(REACHABILITY_EDGE_DELETE, WITH_TREE_MIDDLE){ EdgeDeleteTest(middle_num_nodes, middle_num_graph, 5); }
// TEST(REACHABILITY_EDGE_RANDOM, WITH_TREE_TINY){ EdgeRandomTest(tiny_num_nodes, tiny_num_graph, 1); }
// TEST(REACHABILITY_EDGE_RANDOM, WITH_TREE_SMALL){ EdgeRandomTest(small_num_nodes, small_num_graph, 5); }
// TEST(REACHABILITY_EDGE_RANDOM, WITH_TREE_MIDDLE){ EdgeRandomTest(middle_num_nodes, middle_num_graph, 5); }

// TEST(REACHABILITY_NODE_INSERT, WITH_TREE_TINY){ NodeInsertTest(tiny_num_nodes, tiny_num_graph, 1); }
// TEST(REACHABILITY_NODE_INSERT, WITH_TREE_SMALL){ NodeInsertTest(small_num_nodes, small_num_graph, 5);}
// TEST(REACHABILITY_NODE_DELETE, WITH_TREE_TINY_AVOID_ROOTS){ node_delete_without_roots_test(tiny_num_nodes, tiny_num_graph, 1); }
// TEST(REACHABILITY_NODE_DELETE, WITH_TREE_SMALL_AVOID_ROOTS){ node_delete_without_roots_test(small_num_nodes, small_num_graph, 5); }
// TEST(REACHABILITY_NODE_DELETE, WITH_TREE_MIDDLE_AVOID_ROOTS){  node_delete_without_roots_test(middle_num_nodes, middle_num_graph, 5);}
// TEST(REACHABILITY_NODE_DELETE, WITH_TREE_TINY){ NodeDeleteTest(tiny_num_nodes, tiny_num_graph, 1); }
// TEST(REACHABILITY_NODE_DELETE, WITH_TREE_SMALL){ NodeDeleteTest(small_num_nodes, small_num_graph, 5); }
// TEST(REACHABILITY_NODE_DELETE, WITH_TREE_MIDDLE){ NodeDeleteTest(middle_num_nodes, middle_num_graph, 5); }

