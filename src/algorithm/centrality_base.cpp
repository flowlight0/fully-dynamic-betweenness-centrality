#include "centrality_base.hpp"
#include <cassert>
#include <queue>
#include <algorithm>
using namespace std;

namespace betweenness_centrality {
  void CentralityBase::BuildGraph(const vector<pair<int, int> > &es){
    // CHECK(!es.empty());
    vertex2id.clear();
    G[0].clear();
    G[1].clear();
    
    if (!es.empty()){
      V = 0;
      E = es.size();
      for (const auto &e : es){
        if (vertex2id.count(e.fst) == 0) vertex2id[e.fst] = V++;
        if (vertex2id.count(e.snd) == 0) vertex2id[e.snd] = V++;
      }
      G[0].resize(V);
      G[1].resize(V);
      for (const auto &e : es){
        int u = vertex2id[e.fst];
        int v = vertex2id[e.snd];
        if (u != v){
          G[0][u].push_back(v);
          G[1][v].push_back(u);
        }
      }
    } else {
      V = 1;
      E = 0;
      G[0].resize(V);
      G[1].resize(V);
      vertex2id[0] = 0;
    }

    for (size_t v = 0; v < V; v++){
      sort(G[0][v].begin(), G[0][v].end());
      sort(G[1][v].begin(), G[1][v].end());
    }
    CHECK(V == vertex2id.size());
  }
}

