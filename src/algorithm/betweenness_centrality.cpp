#include "betweenness_centrality.hpp"
#include <cassert>
#include <queue>
#include <algorithm>
using namespace std;

namespace betweenness_centrality {
  void BetweennessCentralityBase::BuildGraph(const vector<pair<int, int> > &es){
    vertex2id.clear();
    G[0].clear();
    G[1].clear();
    
    V = 0;
    E = es.size();
    for (const auto &e : es){
      if (vertex2id.count(e.fst) == 0) vertex2id[e.fst] = V++;
      if (vertex2id.count(e.snd) == 0) vertex2id[e.snd] = V++;
    }
    G[0].resize(V);
    G[1].resize(V);
    for (const auto &e : es){
      G[0][vertex2id[e.fst]].push_back(vertex2id[e.snd]);
      G[1][vertex2id[e.snd]].push_back(vertex2id[e.fst]);
    }
  }
  
  void BetweennessCentralityNaive::PreCompute(const vector<pair<int, int> > &es) {
    BuildGraph(es);
    centrality_map = vector<double>(V, 0);
    const auto &forward_adj = G[0];
    
    for (int s = 0; (size_t)s < V; s++){
      // Brandes' algorithm (2001)
      // Step1. Compute distance and the number of shortest paths from s. O(m) time.
      vector<int>    distance(V, -1);
      vector<double> num_paths(V, 0);
      vector<double> delta(V, 0);
      vector<int>    order;
      queue<int>     que;
      
      distance[s] = 0;
      num_paths[s] = 1;
      que.push(s);
      
      while (!que.empty()){
        int v = que.front(); que.pop(); order.push_back(v);
        for (int w : forward_adj[v]){
          if (distance[w] == -1){
            distance[w] = distance[v] + 1;;
            que.push(w);
          }
          
          if (distance[w] == distance[v] + 1){
            num_paths[w] += num_paths[v];
          }
        }
      }
      
      // Step2. Aggregate betweenness centrality values. O(m) time.
      reverse(order.begin(), order.end());
      for (int v : order){
        for (int w : forward_adj[v]){
          if (distance[w] == distance[v] + 1 && num_paths[w] > 0){
            delta[v] += num_paths[v] / num_paths[w];
            delta[v] += num_paths[v] / num_paths[w] * delta[w];
          }
        }
      }
      for (int t = 0; (size_t)t < V; t++){
        if (s != t) centrality_map[t] += delta[t];
      }
    }
  }
  
  
  
  // void BetweennessCentrality::Index::
  // calcPaths(int s, const vector<IVec> &dag, DVec &num_ps){
  //   assert(num_ps.size() == dag.size() && 0 <= s && s < int(dag.size()));
  //   fill(num_ps.begin(), num_ps.end(), 0);
  //   queue<int> que;
  //   num_ps[s] = 1;
  //   que.push(s);
    
  //   while (!que.empty()){
  //     int v = que.front(); que.pop();
  //     for (int w : dag[v]){  // dag[v]は最短路DAGの辺なので距離のチェックは不要
  //       if (num_ps[w] < EPS) que.push(w);
  //       num_ps[w] += num_ps[v];
  //     }
  //   }
  // }

  
  // inline double StaticSketch::Index::getWeight(int id) const {
  //   if (0 <= id && id < int(id_to_vertex.size()) && id != sources[0] && id != sources[1]){
  //     return num_ps[0][id] / num_total_paths * num_ps[1][id];
  //   } else {
  //     return 0;
  //   }
  // }
  
  // StaticSketch::Index::Index(int s, int t,
  //                            const vector<IVec> &dag_fadj,
  //                            const vector<IVec> &dag_badj,
  //                            const IVec &id_to_vertex)
  //   : sources({s, t}), adj({dag_fadj, dag_badj}), id_to_vertex(id_to_vertex)
  // {
  //   int n = id_to_vertex.size();
  //   // assert(s < int(adj[0].size()));

  //   REP(i, 2){
  //     num_ps[i].resize(n, 0);
  //     calcPaths(sources[i], adj[i], num_ps[i]);
  //   }
    
  //   double abs_error = abs(num_ps[0][t] - num_ps[1][s]);
  //   double max_abs   = max(num_ps[0][t], num_ps[1][s]);
  //   assert(max_abs < EPS || abs_error / max_abs < EPS);
  //   num_total_paths = num_ps[0][t];
  //   assert(num_total_paths < 1e100);
  //   assert(num_total_paths > 0);
  // }
  
  // StaticSketch::Index *StaticSketch::buildIndex(int s, int t){
  //   vector<int> &id_to_vertex = tmp_vars.id_to_vertex;
  //   vector<int> &vertex_to_id = tmp_vars.vertex_to_id;
    
  //   assert(id_to_vertex.empty());
  //   bidirectionalSearch(s, t);
    
  //   if (!id_to_vertex.empty()){
  //     vector<IVec > dag_fadj(id_to_vertex.size());
  //     vector<IVec > dag_badj(id_to_vertex.size());
  //     assert(vertex_to_id[s] != -1 && vertex_to_id[t] != -1);
      
  //     for (auto v : id_to_vertex){
  //       for (auto w : tmp_vars.adj[v]){
  //         dag_fadj[vertex_to_id[v]].push_back(vertex_to_id[w]);
  //         dag_badj[vertex_to_id[w]].push_back(vertex_to_id[v]);
  //       }
  //     }
      
  //     int s_id = vertex_to_id[s];
  //     int t_id = vertex_to_id[t];
  //     for (int v : id_to_vertex){
  //       tmp_vars.adj[v].clear();
  //       vertex_to_id[v] = -1;
  //     }
  //     for (auto v : vertex_to_id) assert(v == -1);

  //     Index *index = new Index(s_id, t_id, dag_fadj, dag_badj, id_to_vertex);
  //     id_to_vertex.clear();
  //     return index;
  //   } else {
  //     return nullptr;
  //   }
  // }

  // inline void StaticSketch::addNodeToDag(int v){
  //   if (tmp_vars.vertex_to_id[v] == -1){
  //     int id = tmp_vars.id_to_vertex.size();
  //     tmp_vars.id_to_vertex.push_back(v);
  //     tmp_vars.vertex_to_id[v] = id;
  //   }
  // }

  // inline void StaticSketch::
  // updateDag(const IVec &dist, queue<int> &que, int v, int w, bool rev){
  //   if (dist[w] != -1 && dist[w] == dist[v] - 1){
  //     if (!tmp_vars.dag_nodes[w]){
  //       tmp_vars.dag_nodes[w] = true;
  //       que.push(w);
  //       addNodeToDag(w);
  //     }
      
  //     if (!rev){
  //       tmp_vars.adj[w].push_back(v);
  //     } else {
  //       tmp_vars.adj[v].push_back(w);
  //     }
  //   }
  // }
  
  // void StaticSketch::bidirectionalSearch(int s, int t){
  //   assert(s != t);
  //   int  curr  = 0;
  //   int  next  = 2;
  //   queue<int>  que[4];
  //   vector<int> update[2];
  //   vector<int> center;
    
  //   que[0].push(s); tmp_vars.dist[0][s] = 0; update[0].push_back(s);
  //   que[1].push(t); tmp_vars.dist[1][t] = 0; update[1].push_back(t);
    
  //   while (!que[curr].empty() || !que[curr + 1].empty()){
  //     REP(i, 2){
  //       while (!que[curr + i].empty()){
  //         int v = que[curr + i].front(); que[curr + i].pop();
  //         for (int w : adj[i][v]){
  //           int &src_d = tmp_vars.dist[i    ][w];
  //           int &dst_d = tmp_vars.dist[1 - i][w];
  //           if (src_d != -1) continue;
  //           if (dst_d != -1) center.push_back(w);
  //           que[next + i].push(w);
  //           update[i].push_back(w);
  //           tmp_vars.dist[i][w] = tmp_vars.dist[i][v] + 1;
  //         }
  //       }
  //       if (!center.empty()) goto LOOP_END;
  //     }
  //     swap(curr, next);
  //   }
  // LOOP_END:
  //   tmp_vars.id_to_vertex.clear();
    
  //   for (int v : center) addNodeToDag(v);
  //   {
  //     queue<int> que;
  //     REP(i, 2){
  //       for (int v : center) que.push(v);
  //       while (!que.empty()){
  //         int v = que.front(); que.pop();
  //         for (int w : adj[1 - i][v]){
  //           updateDag(tmp_vars.dist[i], que, v, w, i);
  //         }
  //       }
  //     }
  //   }

  //   for (int v :tmp_vars.id_to_vertex) tmp_vars.dag_nodes[v] = false;
    
  //   REP(i, 2) {
  //     for (auto v : update[i]) tmp_vars.dist[i][v] = -1;
  //   }
  // }
  
  // void StaticSketch::init(){
  //   // adjacecy list以外のメンバ変数を初期化
  //   centrality = std::vector<double>(num_nodes, 0.0);
  //   tmp_vars.init(num_nodes);
  //   indices.clear();
  // }
  
  // void StaticSketch::constructSketch(){
    
  //   if (num_indices == -1){
  //     // test用のコード、厳密解をすべての頂点に対してサンプリングを行って求める
  //     num_indices = num_nodes * num_nodes;
      
  //     REP(s, num_nodes) REP(t, num_nodes){
  //       Index *e = nullptr;
  //       if (s != t && (e = buildIndex(s, t)) != nullptr){
  //         REP(j, e->getDagSize()){
  //           centrality[e->getNode(j)] += e->getWeight(j);
  //         }
  //       }
  //       indices.push_back(e);
  //     }
  //   } else {
  //     // 通常のインデックス作成
  //     REP(i, num_indices){
  //       int s = rand() % num_nodes;
  //       int t = rand() % num_nodes;
  //       Index *e = nullptr;
        
  //       if (s != t && (e = buildIndex(s, t)) != nullptr){
  //         REP(j, e->getDagSize()){
  //           centrality[e->getNode(j)] += e->getWeight(j);
  //         }
  //       }
  //       indices.push_back(e);
  //     }
  //   }
  // }
  
  // void StaticSketch::Compute(const vector<pair<int, int> > &es, int num_samples){
  //   // Only consider undirected graphs.
  //   int V = 0;
  //   for (const auto &e : es){
  //     V = max({V, e.fst + 1, e.snd + 1});
  //   }
    
  //   vector<vector<int> > G(V);
  //   for (const auto &e : es){
  //     G[e.fst].push_back(e.snd);
  //     G[e.snd].push_back(e.fst);
  //   }

  //   vector<vector<int> > dist(2, vector<int>(V));
    
    
    
    
  //   clear();
  //   num_nodes = computeNumNodes(es);
  //   num_indices = M;

  //   // setup adjacency lists.
  //   REP(i, 2){
  //     adj[i].resize(num_nodes);
  //   }
  //   for (auto &e : es){
  //     adj[0][e.first].push_back(e.second);
  //     adj[1][e.second].push_back(e.first);
  //   }

  //   REP(i, 2) REP(v, num_nodes){
  //     sort(ALL(adj[i][v]));
  //     make_unique<int>(adj[i][v]);
  //   }
    
  //   init();
  //   constructSketch();
  // }
}

