#ifndef DYNAMIC_CENTRALITY_NAIVE_H
#define DYNAMIC_CENTRALITY_NAIVE_H

#include "centrality_brandes.hpp"
#include "dynamic_centrality_base.hpp"
#include <vector>
#include <cstdlib>
using std::vector;
using std::pair;

namespace betweenness_centrality {

  class DynamicCentralityNaive : public DynamicCentralityBase {
    vector<pair<int, int> > es;
    CentralityBrandes cn;
  
  public:
    void PreCompute(const vector<pair<int, int> > &es, int num_samples = -1);
  
    virtual double QueryCentrality(int v) const {
      return vertex2id.count(v) ? cn.QueryCentrality(vertex2id.at(v)) : 0;
    }
    virtual void InsertNode(int v);
    virtual void DeleteNode(int v);
    virtual void InsertEdge(int u, int v);
    virtual void DeleteEdge(int u, int v);
  };
}

#endif /* DYNAMIC_CENTRALITY_NAIVE_H */
