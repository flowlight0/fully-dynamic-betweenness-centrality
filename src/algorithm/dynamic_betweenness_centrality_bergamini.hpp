#ifndef DYNAMIC_BETWEENNESS_CENTRALITY_BERGAMINI_H
#define DYNAMIC_BETWEENNESS_CENTRALITY_BERGAMINI_H


#include "common.hpp"
#include "dynamic_betweenness_centrality.hpp"
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
  
  class DynamicSPT {
    int V;
    vector<short> distance;
    vector<float> num_ps;
    vector<vector<int> > prev_nodes;
    vector<Color> color;

    const int  source;
    const vector<vector<int> > *fadj;
    const vector<vector<int> > *badj;
    const bool store_prev;
    bool  mod;
    
  public:
    DynamicSPT(int source,
               vector<vector<int> > *fadj,
               vector<vector<int> > *badj,
               bool store_prev);
    virtual ~DynamicSPT(){}
    
    void insertEdge(const vector<pair<int ,int> > &es);
    void sampleSP(int target, vector<int> &sp);
    inline int getDistance(int v) const { return distance[v]; }
    inline double getNumber(int v) const { return num_ps[v]; }
    inline bool modified() const { return mod; }
    size_t getDataSize() const ;
  private: 
    inline bool validNode(int v) { return 0 <= v && v < V; }
  };
  
  class DynamicBetweennessCentralityBergamini : public DynamicBetweennessCentralityBase {
    int num_samples;
    vector<double> score;
    vector<int> sources;
    vector<int> targets;
    vector<DynamicSPT> SPTs;
    vector<vector<int> > SPs;
    
  public:
    DynamicBetweennessCentralityBergamini() {}
    virtual ~DynamicBetweennessCentralityBergamini(){}
    virtual void PreCompute(const vector<pair<int, int> > &es, int num_samples = -1);
    void BatchInsert(const vector<pair<int, int> > &es);
    
    virtual void InsertNode(int) { assert(false); }
    virtual void DeleteNode(int) { assert(false); }
    virtual void InsertEdge(int u, int v);
    virtual void DeleteEdge(int ,int) { assert(false); }
    virtual inline double QueryCentrality(int v) const ;
    // const { return score[v];}
  private:
  };

  double DynamicBetweennessCentralityBergamini::QueryCentrality(int v) const {
    if (vertex2id.count(v)){
      return score[vertex2id.at(v)] * V * V;
    } else {
      return 0;
    }
  }
}
  
#endif /* DYNAMIC_BETWEENNESS_CENTRALITY_BERGAMINI_H */

