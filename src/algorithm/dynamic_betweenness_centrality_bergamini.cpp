#include "dynamic_betweenness_centrality_bergamini.hpp"
#include <queue>
#include <cassert>
using namespace std;

namespace betweenness_centrality {
  DynamicSPT::DynamicSPT(int source, vector<vector<int> > *fadj, vector<vector<int> > *badj, bool store_prev)
    : source(source), fadj(fadj), badj(badj), store_prev(store_prev)
  {
    V = fadj->size();
    distance.resize(V, INF);
    num_ps.resize(V, 0);
    color.resize(V, WHITE);
    if (store_prev){
      prev_nodes.resize(V);
    }

    queue<int> que;
    distance[source] = 0;
    num_ps[source] = 1;
    que.push(source);

    while (!que.empty()){
      int v = que.front(); que.pop();
      CHECK(v == source || num_ps[v] == 0.0);

      for (int u : badj->at(v)){
        if (distance[u] + 1 == distance[v]){
          num_ps[v] += num_ps[u];
          if (store_prev){
            prev_nodes[v].push_back(u);
          }
          CHECK(u != v && 0 <= u && u < V);
          CHECK(num_ps[v] < std::numeric_limits<float>::max() / 2);
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

  void DynamicSPT::insertEdge(const vector<pair<int, int> > &es){
    mod = false;
    short max_dist = 0;
    for (const auto &e: es){
      if (distance[e.first ] < INF) max_dist = max(max_dist, distance[e.first ]);
      if (distance[e.second] < INF) max_dist = max(max_dist, distance[e.second]);
    }

    // queues[max_dist + 1] will be required.
    vector<queue<int> > queues(max_dist + 2);    

    for (const auto &e: es){
      // if (distance[e.first]  >= distance[e.second] + 1){
      //   queues[distance[e.second] + 1].push(e.first );
      // }
      if (distance[e.second] >= distance[e.first ] + 1){
        queues[distance[e.first ] + 1].push(e.second);
      }
    }

    // cerr << "START" << endl;
    vector<int> visited;
    for (size_t m = 1; m < queues.size(); m++){
      while (!queues[m].empty()){
        mod = true;
        int v = queues[m].front();
        queues[m].pop();
        
        if (color[v] == BLACK){
          continue;
        }

        visited.push_back(v);
        color[v] = BLACK;

        distance[v] = m;
        num_ps[v]   = 0;
        if (store_prev){
          prev_nodes[v].clear();
        }

        for (int u : badj->at(v)){
          if (distance[v] == distance[u] + 1){
            if (store_prev){
              prev_nodes[v].push_back(u);
            }
            num_ps[v] += num_ps[u];
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

  void DynamicSPT::sampleSP(int target, vector<int> &ps){
    CHECK(validNode(target) && distance[target] != INF);
    ps.clear();
    ps.push_back(target);
    for (int v = target; v != source; ps.push_back(v)){
      vector<pair<int, double> > choices;
      CHECK(validNode(v));
      
      if (store_prev){
        for (int w : prev_nodes[v]){
          double weight = getNumber(w) / getNumber(v);
          choices.emplace_back(w, weight);
        }
      } else {
        for (int w : badj->at(v)){
          if (distance[w] + 1 == distance[v]){
            double weight = getNumber(w) / getNumber(v);
            choices.emplace_back(w, weight);
          }
        }
      }
      CHECK(!choices.empty());

      double total = 0;
      for (const auto &p : choices){
        total += p.second;
      }

      if (!(abs(total - 1.0) < 1e-5)){
        for (const auto &p : choices){
          cerr << p <<" "<< distance[p.first] <<" "<< num_ps[p.first] << endl;
        }
        cerr << total << " " << v << " " << distance[v] << endl;
      }
      CHECK(abs(total - 1.0) < 1e-5);

      double r = (double)rand() / RAND_MAX;
      v = -1;
      for (const auto &p : choices){
        if (r < p.second){
          v = p.first;
          break;
        }
        r -= p.second;
      }
      CHECK(v != -1);
    }
    reverse(ps.begin(), ps.end());
  }
  
  void DynamicBetweennessCentralityBergamini::PreCompute(const vector<pair<int, int> > &es, int num_samples_){
    this->num_samples = num_samples_;
    BuildGraph(es);
    sources.resize(num_samples);
    targets.resize(num_samples);
    score.resize(V);
    
    for (int i = 0; i < num_samples; i++){
      sources[i] = rand() % V;
      targets[i] = rand() % V;
      // We do not keep predecessor of each node in each shortest path tree.
      DynamicSPT  spt(sources[i], &G[0], &G[1], false); 
      vector<int> sp;

      if (spt.getDistance(targets[i]) != INF){
        spt.sampleSP(targets[i], sp);
      }

      for (int v : sp){
        if (v != sources[i] && v != targets[i]) score[v] += 1.0 / num_samples;
      }
      SPTs.push_back(spt);
      SPs.push_back(sp);
    }
  }
  
  void DynamicBetweennessCentralityBergamini::BatchInsert(const vector<pair<int, int> > &es){
    for (const auto &e : es){
      G[0][e.first ].push_back(e.second);
      G[1][e.second].push_back(e.first );
    }

    for (int i = 0; i < num_samples; i++){
      SPTs[i].insertEdge(es);
      if (SPTs[i].modified() && SPTs[i].getDistance(targets[i]) != INF){
        vector<int> new_sp;
        SPTs[i].sampleSP(targets[i], new_sp);

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

  void DynamicBetweennessCentralityBergamini::InsertEdge(int u, int v){
    vector<pair<int, int> > es = {make_pair(u, v)};
    BatchInsert(es);
  }
}
