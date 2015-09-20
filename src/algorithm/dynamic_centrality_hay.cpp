// #include "dynamic_index.hpp"
#include "dynamic_centrality_hay.hpp"
using namespace std;
using namespace betweenness_centrality::special_purpose_reachability_index;

namespace betweenness_centrality {
  vector<pair<int, int> > DynamicCentralityHAY::SampleVertexPairs(){
    vector<pair<int, int> > res;
    if (debug_mode){
      for (int s = 0; (size_t)s < V; s++){
        for (int t = 0; (size_t)t < V; t++){
          res.emplace_back(s, t);
        }
      }
    } else {
      for (int i = 0; i < num_samples; i++){
        res.emplace_back(rand() % V, rand() % V);
      }
    }
    return res;
  }

  // グラフ以外の部分を初期化
  void DynamicCentralityHAY::Init(){
    spr_index = new SpecialPurposeReachabilityIndex(&G[0], &G[1], 10);
    total_weight = vector<double>(V, 0);
    for (int i = 0; i < 2; i++){
      tmp_dist[i]  = vector<int>(V, -1);
      tmp_count[i] = vector<double>(V, 0);
    }
    tmp_passable.resize(V, false);
  }
  
  void DynamicCentralityHAY::Clear(){
    SafeDelete(spr_index);
    
    for (auto &index : hyper_edges) SafeDelete(index);
    G[0].clear();
    G[1].clear();
    total_weight.clear();
    hyper_edges.clear();
    
    for (int i = 0; i < 2; i++){
      tmp_dist[i].clear();
      tmp_count[i].clear();
    }
    tmp_passable.clear();
  }
  
  void DynamicCentralityHAY::
  PreCompute(const vector<pair<int, int> > &es, int num_samples_){    
    Clear();
    this->num_samples = num_samples_;
    if (num_samples == -1){
      debug_mode = true;
    } else {
      debug_mode = false;
    }
    
    BuildGraph(es);
    Init();
    
    auto vertex_pairs = SampleVertexPairs();
    for (const auto &vp : vertex_pairs){
      hyper_edges.push_back(new HyperEdge(vp.fst, vp.snd, this));
    }
  }

  // 辺 {s, t}がすでにあった場合は何もせずfalseをかえす
  bool DynamicCentralityHAY::InsertEdgeIntoGraph(int s, int t){
    auto &f_adj = G[0];
    auto &b_adj = G[1];
    
    auto fiter = lower_bound(f_adj[s].begin(), f_adj[s].end(), t);
    if (fiter != f_adj[s].end() && *fiter == t){
      return false; 
    } else {
      auto biter = lower_bound(b_adj[t].begin(), b_adj[t].end(), s);
      f_adj[s].insert(fiter, t);
      b_adj[t].insert(biter, s);
      return true;
    }
  }
  
  // 辺 {s, t}が存在しない場合何もしないでfalseを返す
  bool DynamicCentralityHAY::DeleteEdgeFromGraph(int s, int t){
    auto &f_adj = G[0];
    auto &b_adj = G[1];
    auto fiter = lower_bound(f_adj[s].begin(), f_adj[s].end(), t);
    if (fiter != f_adj[s].end() && *fiter == t){
      auto biter = lower_bound(b_adj[t].begin(), b_adj[t].end(), s);
      f_adj[s].erase(fiter);
      b_adj[t].erase(biter);
      return true;
    } else {
      return false;  
    } 
  }

  bool DynamicCentralityHAY::InsertNodeIntoGraph(int v){
    if (vertex2id.count(v)){
      return false;
    } else {
      CHECK(vertex2id.size() == V);
      vertex2id[v] = V++;

      while (G[0].size() < vertex2id.size()){
        G[0].push_back(vector<int>());
        G[1].push_back(vector<int>());
        total_weight.push_back(0);
        for (int i = 0; i < 2; i++){
          tmp_dist[i].push_back(-1);
          tmp_count[i].push_back(0);
        }
        tmp_passable.push_back(false);
      }
      CHECK(G[0].size() == V);
      return true;
    }
  }

  bool DynamicCentralityHAY::DeleteNodeFromGraph(int u){
    // CHECK(active[u]);
    // active[u] = false;
    // active_count--;
    
    // vector<int> u_out(G[0][u]);
    // vector<int> u_in(G[1][u]);

    // for (int v : u_out){
    //   CHECK(u != v);
    //   auto iter = lower_bound(G[1][v].begin(), G[1][v].end(), u);
    //   CHECK(iter != G[1][v].end() && *iter == u);
    //   G[1][v].erase(iter);
    // }
    
    // for (int v : u_in){
    //   CHECK(u != v);
    //   auto iter = lower_bound(G[0][v].begin(), G[0][v].end(), u);
    //   CHECK(iter != G[0][v].end() && *iter == u);
    //   G[0][v].erase(iter);
    // }
    // G[0][u].clear();
    // G[1][u].clear();
    return true;
  }

  void DynamicCentralityHAY::InsertEdge(int s, int t){
    CHECK(vertex2id.count(s) && vertex2id.count(t));
    s = vertex2id[s];
    t = vertex2id[t];
    if (InsertEdgeIntoGraph(s, t)){
      spr_index->InsertEdge(s, t);
      for (auto e : hyper_edges){
        e->InsertEdge(s, t);
      }
    }
  }

  void DynamicCentralityHAY::DeleteEdge(int s, int t){
    CHECK(vertex2id.count(s) && vertex2id.count(t));
    s = vertex2id[s];
    t = vertex2id[t];
    if (DeleteEdgeFromGraph(s, t)){
      spr_index->DeleteEdge(s, t);
      for (auto &e : hyper_edges){
        e->DeleteEdge(s, t);
      }
    }
  }
  
  void DynamicCentralityHAY::InsertNode(int u){
    if (InsertNodeIntoGraph(u) && ValidNode(u)){
      // cout << "VALID INSERTIONN: " << V << " " << G[0] << endl;
      u = vertex2id[u];
      CHECK((size_t)u + 1 == V);
      spr_index->InsertNode(u);
      
      if (debug_mode){
        for (size_t s = 0; s < V; s++)
          for (size_t t = 0; t < V; t++)
            if (s >= V - 1 || t >= V - 1)
              hyper_edges.push_back(new HyperEdge(s, t, this));
      } else {
        // CHECK(active_count > 1);
        CHECK(V > 0);
        double prob1 = (double)(V - 1) / V * (V - 1) / V;;
        double prob2 = 1.0 / V / V;
        
        // cout << prob1 << " " << prob2 << " " << q << endl;
          
        for (auto &e : hyper_edges){
          int new_source = -1;
          int new_target = -1;
          double q = (double)rand() / RAND_MAX;
          if (q < prob1){ // no change
            continue;
          } else if (q < prob2 + prob1){
            new_source = new_target = u; // re-sample pair of same vertices (really low prob.)
          } else {
            new_source = new_target = u;
            CHECK(V > 1u);
            while (new_target == u){
              new_target = rand() % V;
            }
            
            if (((double)rand() / RAND_MAX) < 0.5){
              swap(new_source, new_target);
            }
          }
          SafeDelete(e);
          e = new HyperEdge(new_source, new_target, this);
        }
      }
      for (auto e : hyper_edges){
        e->InsertNode(u);
      }
    }
  }
  
  void DynamicCentralityHAY::DeleteNode(int u){
    // 内部では頂点数の変更を行わないので媒介中心性の値は適当な重みをかける必要あり
    CHECK(0 <= u && (size_t)u < V);
    // vector<int> u_out(G[0].at(u));
    // vector<int> u_in (G[1].at(u));
    
    // if (DeleteNodeFromGraph(u)){
    //   spr_index->DeleteNode(u, u_out, u_in);
      
    //   if (debug_mode){
    //     for (auto e : hyper_edges) e->DeleteNode(u, u_out, u_in);
    //   } else {
    //     for (auto &e : hyper_edges){
    //       if (e->GetSource() == u || e->GetTarget() == u){
    //         CHECK(active_count > 0);
    //         int new_source = u;
    //         int new_target = u;
    //         while (!active[new_target] || !active[new_source]){
    //           new_source = rand() % V;
    //           new_target = rand() % V;
    //         }
    //         CHECK(new_target != u && new_source != u);
    //         SafeDelete(e);
    //         e = new HyperEdge(new_source, new_target, this);
    //       } else {
    //         e->DeleteNode(u, u_out, u_in);
    //       }
    //     }
    //   }
    //   CHECK(total_weight[u] < 1e-9);
    // }
  }

} /* betweenness_centrality */
