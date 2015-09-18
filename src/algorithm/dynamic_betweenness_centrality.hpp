#ifndef DYNAMIC_BETWEENNESS_CENTRALITY_H
#define DYNAMIC_BETWEENNESS_CENTRALITY_H

#include "betweenness_centrality.hpp"

namespace betweenness_centrality {
  class DynamicBetweennessCentralityBase
    : public betweenness_centrality::BetweennessCentralityBase { 
  public:
    virtual void InsertNode(int v) = 0;
    virtual void DeleteNode(int v) = 0;
    virtual void InsertEdge(int u, int v) = 0;
    virtual void DeleteEdge(int u, int v) = 0;
  };
}

#endif /* DYNAMIC_BETWEENNESS_CENTRALITY_H */
