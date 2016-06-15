#include "dynamic_centrality_bms.hpp"
#include <queue>
#include <cassert>
using namespace std;

namespace betweenness_centrality {
  
  void DynamicShortestPathTree::Resize(){
    V = fadj->size();
    size_t curr_size = distance.size();
    while (curr_size++ < V){
      distance.push_back(INF);
      num_paths.push_back(0);
      prev_nodes.push_back(vector<int>());
      color.push_back(WHITE);
    }
  }
  
  DynamicShortestPathTree::DynamicShortestPathTree(int source, vector<vector<int> > *fadj, vector<vector<int> > *badj, bool store_prev)
    : source(source), fadj(fadj), badj(badj), store_prev(store_prev)
  {
    V = fadj->size();
    distance.resize(V, INF);
    num_paths.resize(V, 0);
    color.resize(V, WHITE);
    prev_nodes.resize(V);
    
    queue<int> que;
    distance[source] = 0;
    num_paths[source] = 1;
    que.push(source);

    while (!que.empty()){
      int v = que.front(); que.pop();
      CHECK(v == source || num_paths[v] == 0.0);

      for (int u : badj->at(v)){
        if (distance[u] + 1 == distance[v]){
          num_paths[v] += num_paths[u];
          if (store_prev){
            prev_nodes[v].push_back(u);
          }
          CHECK(u != v && 0 <= u && (size_t)u < V);
          CHECK(num_paths[v] < std::numeric_limits<float>::max() / 2);
        }
      }

      for (int w : fadj->at(v)){
        if (distance[w] > distance[v] + 1){
          distance[w] = distance[v] + 1;
          que.push(w);
        }

      }
    }
  }
  
  void DynamicShortestPathTree::InsertEdge(const vector<pair<int, int> > &es){
    Resize();
    
    modified = false;
    short max_dist = 0;
    for (const auto &e: es){
      if (distance[e.fst] < INF) max_dist = max(max_dist, distance[e.fst]);
      if (distance[e.snd] < INF) max_dist = max(max_dist, distance[e.snd]);
    }

    // queues[max_dist + 1] will be required.
    vector<queue<int> > queues(max_dist + 2);    
    
    for (const auto &e: es){
      // if (distance[e.fst]  >= distance[e.snd] + 1){
      //   queues[distance[e.snd] + 1].push(e.fst);
      // }
      if (distance[e.snd] >= distance[e.fst ] + 1){
        queues[distance[e.fst ] + 1].push(e.snd);
      }
    }

    // cerr << "START" << endl;
    vector<int> visited;
    for (size_t m = 1; m < queues.size(); m++){
      while (!queues[m].empty()){
        modified = true;
        int v = queues[m].front();
        queues[m].pop();
        
        if (color[v] == BLACK){
          continue;
        }

        visited.push_back(v);
        color[v] = BLACK;

        distance[v] = m;
        num_paths[v]   = 0;
        if (store_prev){
          prev_nodes[v].clear();
        }

        for (int u : badj->at(v)){
          if (distance[v] == distance[u] + 1){
            if (store_prev){
              prev_nodes[v].push_back(u);
            }
            num_paths[v] += num_paths[u];
          }
        }
        // DEBUG(badj->at(v));

        for (int w : fadj->at(v)){
          if (color[w] == WHITE && distance[w] >= distance[v] + 1){
            color[w] = GRAY;
            if (m + 1 == queues.size()){
              queues.push_back(std::queue<int>());
            }
            queues[m + 1].push(w);
          }
        }
      }
    }

    for (int v : visited){
      color[v] = WHITE;
    }
    visited.clear();
  }

  void DynamicShortestPathTree::SampleSP(int target, vector<int> &ps){
    CHECK(ValidNode(target) && distance[target] != INF);
    ps.clear();
    ps.push_back(target);
    for (int v = target; v != source; ps.push_back(v)){
      vector<pair<int, double> > choices;
      CHECK(ValidNode(v));
      
      if (store_prev){
        for (int w : prev_nodes[v]){
          double weight = GetNumPaths(w) / GetNumPaths(v);
          choices.emplace_back(w, weight);
        }
      } else {
        for (int w : badj->at(v)){
          if (distance[w] + 1 == distance[v]){
            double weight = GetNumPaths(w) / GetNumPaths(v);
            choices.emplace_back(w, weight);
          }
        }
      }
      CHECK(!choices.empty());

      double total = 0;
      for (const auto &p : choices){
        total += p.snd;
      }

      if (!(abs(total - 1.0) < 1e-5)){
        for (const auto &p : choices){
          cerr << p <<" "<< distance[p.fst] <<" "<< num_paths[p.fst] << endl;
        }
        cerr << total << " " << v << " " << distance[v] << endl;
      }
      CHECK(abs(total - 1.0) < 1e-5);

      double r = (double)rand() / RAND_MAX;
      v = -1;
      for (const auto &p : choices){
        if (r < p.snd){
          v = p.fst;
          break;
        }
        r -= p.snd;
      }
      CHECK(v != -1);
    }
    reverse(ps.begin(), ps.end());
  }

  void DynamicCentralityBMS::PreCompute(const vector<pair<int, int> > &es, int num_samples_){
    this->num_samples = num_samples_;
    BuildGraph(es);
    sources.resize(num_samples);
    targets.resize(num_samples);
    score.resize(V);
    
    for (int i = 0; i < num_samples; i++){
      sources[i] = rand() % V;
      targets[i] = rand() % V;
      // We do not keep predecessor of each node in each shortest path tree.
      DynamicShortestPathTree  spt(sources[i], &G[0], &G[1], false); 
      vector<int> sp;
      
      if (spt.GetDistance(targets[i]) != INF){
        spt.SampleSP(targets[i], sp);
      }

      for (int v : sp){
        if (v != sources[i] && v != targets[i]) score[v] += 1.0 / num_samples;
      }
      SPTs.push_back(spt);
      SPs.push_back(sp);
    }
  }
  
  void DynamicCentralityBMS::BatchInsert(const vector<pair<int, int> > &es_){
    assert(num_samples >= 1);
    vector<pair<int, int> > es;
    for (const auto &e : es_){
      CHECK(vertex2id.count(e.fst) == 1);
      CHECK(vertex2id.count(e.snd) == 1);
      // if (vertex2id.count(e.fst) == 0) vertex2id[e.fst] = V++;
      // if (vertex2id.count(e.snd) == 0) vertex2id[e.snd] = V++;
      es.emplace_back(vertex2id[e.fst], vertex2id[e.snd]);
    }
    Resize();
    
    CHECK(G[0].size() == V && G[1].size() == V);
    for (const auto &e : es){
      CHECK((size_t)e.fst < V && (size_t)e.snd < V);
      G[0][e.fst].push_back(e.snd);
      G[1][e.snd].push_back(e.fst);
    }
    
    CHECK(score.size() == V);
    for (int i = 0; i < num_samples; i++){
      SPTs[i].InsertEdge(es);
      if (SPTs[i].Modified() && SPTs[i].GetDistance(targets[i]) != INF){
        vector<int> new_sp;
        SPTs[i].SampleSP(targets[i], new_sp);

        for (int v : SPs[i]){
          if (v != sources[i] && v != targets[i]) score[v] -= 1.0 / num_samples;
        }
        
        for (int v : new_sp){
          if (v != sources[i] && v != targets[i]) score[v] += 1.0 / num_samples;
        }
        SPs[i] = new_sp;
      }
    }
  }
  
  void DynamicCentralityBMS::InsertEdge(int u, int v){
    vector<pair<int, int> > es = {make_pair(u, v)};
    BatchInsert(es);
  }
  
  void DynamicCentralityBMS::Resize(){
    size_t curr_size = score.size();
    while (curr_size++ < V){
      score.push_back(0);
      G[0].push_back(vector<int>());
      G[1].push_back(vector<int>());
    }
  }
}
