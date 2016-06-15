#ifndef DYNAMIC_CENTRALITY_BASE_H
#define DYNAMIC_CENTRALITY_BASE_H

#include "centrality_base.hpp"

namespace betweenness_centrality {
  class DynamicCentralityBase
    : public CentralityBase { 
  public:
    virtual double QueryCentrality(int v) const = 0;
    virtual void InsertNode(int v) = 0;
    virtual void DeleteNode(int v) = 0;
    virtual void InsertEdge(int u, int v) = 0;
    virtual void DeleteEdge(int u, int v) = 0;
  };
}

#endif /* DYNAMIC_CENTRALITY_BASE_H */
