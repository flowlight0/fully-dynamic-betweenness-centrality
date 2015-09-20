#ifndef DYNAMIC_CENTRALITY_BMS_H
#define DYNAMIC_CENTRALITY_BMS_H


#include "common.hpp"
#include "dynamic_centrality.hpp"
#include <climits>
#include <cassert>
#include <algorithm>
using std::vector;
using std::pair;

namespace betweenness_centrality {
  typedef vector<int> path_t;
  typedef char Color;
  constexpr int INF = std::numeric_limits<short>::max() / 2;
  const Color WHITE = 0;
  const Color GRAY  = 1;
  const Color BLACK = 2;
  
  class DynamicShortestPathTree {
    size_t V;
    vector<short> distance;
    vector<float> num_paths;
    vector<vector<int> > prev_nodes;
    vector<Color> color;

    const int  source;
    const vector<vector<int> > *fadj;
    const vector<vector<int> > *badj;
    const bool store_prev;
    bool  mod;
    
  public:
    DynamicShortestPathTree(int source,
                            vector<vector<int> > *fadj,
                            vector<vector<int> > *badj,
                            bool store_prev);
    virtual ~DynamicShortestPathTree(){}
    
    void InsertEdge(const vector<pair<int ,int> > &es);
    void sampleSP(int target, vector<int> &sp);
    inline int GetDistance(int v) const { return distance[v]; }
    inline double GetNumPaths(int v) const { return num_paths[v]; }
    inline bool modified() const { return mod; }
  private:
    void Resize();
    inline bool validNode(int v) { return 0 <= v && (size_t)v < V; }
  };
  
  class DynamicCentralityBMS : public DynamicCentralityBase {
    int num_samples;
    vector<double> score;
    vector<int> sources;
    vector<int> targets;
    vector<DynamicShortestPathTree> SPTs;
    vector<vector<int> > SPs;
    
  public:
    DynamicCentralityBMS() {}
    virtual ~DynamicCentralityBMS(){}
    virtual void PreCompute(const vector<pair<int, int> > &es, int num_samples = -1);
    void BatchInsert(const vector<pair<int, int> > &es);
    
    virtual void InsertNode(int) { assert(false); }
    virtual void DeleteNode(int) { assert(false); }
    virtual void InsertEdge(int u, int v);
    virtual void DeleteEdge(int ,int) { assert(false); }
    virtual double QueryCentrality(int v) const {
      return vertex2id.count(v) ? score[vertex2id.at(v)] * V * V  : 0;
    }
  private:
    void Resize();
  };
}
  
#endif /* DYNAMIC_CENTRALITY_BMS_H */

