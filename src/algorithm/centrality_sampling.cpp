#ifndef CENTRALITY_SAMPLE_H
#define CENTRALITY_SAMPLE_H

#include "common.hpp"
#include "centrality_sampling.hpp"
#include <cassert>
#include <queue>
#include <algorithm>
using namespace std;

namespace betweenness_centrality {
  void CentralitySampling::
  PreCompute(const vector<pair<int, int> > &es, int num_samples){
    BuildGraph(es);
    centrality_map = vector<double>(V, 0);
    temp_on_DAG    = vector<bool>(V, 0);
    temp_distance  = vector<vector<int> > (2, vector<int>(V, -1));
    temp_num_paths = vector<vector<double> > (2, vector<double>(V, 0));
    
    for (int k = 0; k < num_samples; k++){
      int source = rand() % V;
      int target = rand() % V;
      // Step1. Compute vertices on shortest paths DAG between a source and a target.
      vector<int> DAG_vertices = ComputeDAG(source, target);
      
      // Step2.
      for (int v : DAG_vertices){
        temp_distance [Forward][v] = temp_distance [Backward][v] = -1;
        temp_num_paths[Forward][v] = temp_num_paths[Backward][v] = 0;
        temp_on_DAG[v] = true;
      }
      
      if (DAG_vertices.empty()){ // There are no paths from source to sink.
        continue;
      }
      
      BreadthFirstSearchOnDAG(source, Forward);
      BreadthFirstSearchOnDAG(target, Backward);

      // Step3.
      for (int v : DAG_vertices){
        if (v != source && v != target) {
          double num = temp_num_paths[Forward][v] * temp_num_paths[Backward][v];
          double tot = temp_num_paths[Forward][target];
          assert(tot > 0);
          centrality_map[v] += num / tot * V / num_samples * V ;
        }
        temp_on_DAG[v] = false;
        temp_distance [0][v] = temp_distance [1][v] = -1;
        temp_num_paths[0][v] = temp_num_paths[1][v] = 0;
      }
    }
  }

  vector<int> CentralitySampling::
  ComputeDAG(int source, int target){
    if (source == target) return vector<int>(1, source);

    // Step 1. Compute distances from s and t by conducting a bidirectional BFS. 
    vector<int> update_vs[2];
    vector<int> center_vs;
    
    
    int         s_curr = Forward, s_next = Forward + 2;
    int         t_curr = Backward, t_next = Backward + 2;
    queue<int>  que[4];
    que[s_curr].push(source); temp_distance[s_curr][source] = 0; update_vs[s_curr].push_back(source);
    que[t_curr].push(target); temp_distance[t_curr][target] = 0; update_vs[t_curr].push_back(target);
    
    bool found = false;
    while (!que[s_curr].empty() && !que[t_curr].empty()){
      int &curr =     update_vs[Forward].size() <= update_vs[Backward].size() ? s_curr : t_curr;
      int &next =     update_vs[Forward].size() <= update_vs[Backward].size() ? s_next : t_next;
      Direction dir = update_vs[Forward].size() <= update_vs[Backward].size() ? Forward :Backward;
      
      while (!que[curr].empty()){
        int v = que[curr].front(); que[curr].pop();
        const auto &forward_adj = G[dir];
        
        for (int w : forward_adj[v]){
          int &src_d = temp_distance[    dir][w];
          int &dst_d = temp_distance[1 - dir][w];
          if (src_d != -1) continue;
          if (dst_d != -1){
            found = true;
            center_vs.push_back(w);
          }
          que[next].push(w);
          update_vs[dir].push_back(w);
          temp_distance[dir][w] = temp_distance[dir][v] + 1;
        }
      }
      if (found) break;
      std::swap(curr, next);
    }

      
    vector<int> DAG_vertices;
    for (Direction dir : {Forward, Backward}){
      // Step2. Compute vertices on shortest paths DAG between source and target.
      auto &distance  = temp_distance[dir];
      const auto &backward_adj = G[dir == Forward ?  Backward : Forward];
      
      std::queue<int> que;
      for (int v : center_vs) {
        que.push(v);
      }
      
      while (!que.empty()){
        int v = que.front(); que.pop();
        
        if (!temp_on_DAG[v]){   // To push vertices in center_vs into DAG_vertices. 
          temp_on_DAG[v] = true;
          DAG_vertices.push_back(v);
        }
        
        for (int w : backward_adj[v]){
          if (distance[w] + 1 == distance[v] && distance[v] > 0 && !temp_on_DAG[w]){
            que.push(w);
            temp_on_DAG[w] = true;
            DAG_vertices.push_back(w);
          }
        }
      }
      
      // Step3. Reset temporal variables.
      for (int v : update_vs[dir]){
        distance[v] = -1;
      }
    }
    return DAG_vertices;
  }

  void CentralitySampling::
  BreadthFirstSearchOnDAG(int source, Direction dir){
    assert(temp_on_DAG[source]);
    auto &distance    = temp_distance [dir];
    auto &num_paths   = temp_num_paths[dir];
    const auto &forward_adj = G[dir];

    
    std::queue<int> que;
    que.push(source);
    distance[source] = 0;
    num_paths[source] = 1.0;

    while (!que.empty()){
      int v = que.front(); que.pop();
      for (int w : forward_adj[v]){
        if (!temp_on_DAG[w]) continue;
        
        if (distance[w] == -1){
          distance[w] = distance[v] + 1;
          que.push(w);
        }
          
        if (distance[w] == distance[v] + 1){
          num_paths[w] += num_paths[v];
        }
      }
    }
  }
};


#endif /* CENTRALITY_SAMPLE_H */
