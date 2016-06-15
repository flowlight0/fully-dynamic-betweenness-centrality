#ifndef CENTRALITY_H
#define CENTRALITY_H

#include "common.hpp"
#include <vector>
#include <unordered_map>
using std::vector;
using std::unordered_map;

namespace betweenness_centrality {

  class CentralityBase {
  protected:
    size_t V;
    size_t E;
    vector<vector<int> > G[2];
    unordered_map<int, int> vertex2id;
    void BuildGraph(const vector<std::pair<int, int> > &es);
    
  public:
    virtual ~CentralityBase(){};
    virtual void PreCompute(const vector<std::pair<int, int> > &es, int num_samples = -1) = 0;
    virtual double QueryCentrality(int v) const = 0;
  };
}

#endif /* CENTRALITY_H */
