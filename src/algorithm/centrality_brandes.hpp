#ifndef CENTRALITY_NAIVE_H
#define CENTRALITY_NAIVE_H

#include "centrality_base.hpp"

namespace betweenness_centrality {
  class CentralityBrandes : public CentralityBase {
    vector<double> centrality_map;
  public:
    virtual void PreCompute(const vector<std::pair<int, int> > &es, int num_samples = -1);
    virtual double QueryCentrality(int v) const {
      return vertex2id.count(v) ? centrality_map[vertex2id.at(v)] : 0;
    }
  };

}

#endif /* CENTRALITY_NAIVE_H */
