#ifndef CENTRALITY_SAMPLING_H
#define CENTRALITY_SAMPLING_H
#include "common.hpp"
#include "centrality_base.hpp"
#include <vector>
#include <unordered_map>
using std::vector;
using std::unordered_map;

namespace betweenness_centrality {

  class CentralitySampling : public CentralityBase {
    enum Direction {
      Forward,
      Backward
    };
    vector<double> centrality_map;
    vector<bool>   temp_on_DAG;
    vector<vector<int> > temp_distance;
    vector<vector<double> > temp_num_paths;
    
    vector<int> ComputeDAG(int source, int target);
    void BreadthFirstSearchOnDAG(int source, Direction dir);
  public:
    virtual void PreCompute(const vector<std::pair<int, int> > &es, int num_samples = -1);
    virtual double QueryCentrality(int v) const {
      return vertex2id.count(v) ? centrality_map[vertex2id.at(v)] : 0;
    }
  };
}

#endif /* CENTRALITY_SAMPLING_H */
