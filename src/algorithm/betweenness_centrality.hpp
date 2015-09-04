#ifndef BETWEENNESS_CENTRALITY_H
#define BETWEENNESS_CENTRALITY_H

#include "../common.hpp"
#include <vector>
#include <unordered_map>
using std::vector;
using std::unordered_map;

namespace betweenness_centrality {

  class BetweennessCentralityBase {
  protected:
    size_t V;
    size_t E;
    vector<vector<int> > G[2];
    unordered_map<int, int> vertex2id;
    void BuildGraph(const vector<std::pair<int, int> > &es);
    
  public:
    virtual ~BetweennessCentralityBase(){};
    virtual void PreCompute(const vector<std::pair<int, int> > &es) = 0;
    virtual inline double QueryCentrality(int v) const = 0;
  };
  
  class BetweennessCentralityNaive : public BetweennessCentralityBase {
    vector<double> centrality_map;
  public:
    virtual void PreCompute(const vector<std::pair<int, int> > &es);
    virtual inline double QueryCentrality(int v) const {
      return vertex2id.count(v) ? centrality_map[vertex2id.at(v)] : 0;
    }
  };
  
  template <int num_samples>
  class BetweennessCentralitySample : public BetweennessCentralityBase {
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
    virtual void PreCompute(const vector<std::pair<int, int> > &es);
    virtual inline double QueryCentrality(int v) const {
      return vertex2id.count(v) ? centrality_map[vertex2id.at(v)] : 0;
    }
  };
};

#include "betweenness_centrality_sample.hpp"


#endif /* BETWEENNESS_CENTRALITY_H */
