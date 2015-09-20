#ifndef DYNAMIC_CENTRALITY_HAY_H
#define DYNAMIC_CENTRALITY_HAY_H


#include "common.hpp"
#include "dynamic_centrality.hpp"
#include "hyper_edge.hpp"
#include "special_purpose_reachability_index.hpp"
#include <vector>
#include <cstdlib>
using std::vector;
using std::pair;

namespace betweenness_centrality {
  
  class DynamicCentralityHAY : public DynamicCentralityBase {
  private:
    bool debug_mode;
    int num_samples;
    int tradeoff_param;
    int active_count;
    std::vector<int> active;
    std::vector<double>     total_weight;
    std::vector<HyperEdge*> hyper_edges;
    
    // temporal variables for index construction
    std::vector<int>    tmp_dist[2];
    std::vector<double> tmp_count[2];
    std::vector<int>    tmp_passable;

    // keep disjoint set union of nodes
    special_purpose_reachability_index::SpecialPurposeReachabilityIndex *spr_index;
    
    // HyperEdge *buildIndex(int s, int t);
    void Init();  // Initialize the arrays
    void Clear(); // Delete the array
    vector<pair<int, int> > SampleVertexPairs();
    
    // void addNewElems(int num_nodes);
    void AddWeight(const HyperEdge *index, bool minus = false);
    
    bool InsertEdgeIntoGraph(int s, int t);
    bool DeleteEdgeFromGraph(int s, int t);
    bool InsertNodeIntoGraph(int v);
    bool DeleteNodeFromGraph(int v);
    inline bool ValidNode(int v) const { return vertex2id.count(v); }
    
  public:
    DynamicCentralityHAY() : debug_mode(false), tradeoff_param(0), spr_index(nullptr) { }
    ~DynamicCentralityHAY(){ Clear(); }
    
    virtual void PreCompute(const vector<pair<int, int> > &es, int num_samples);
    
    virtual double QueryCentrality(int v) const {
      return ValidNode(v) ? total_weight[vertex2id.at(v)] / hyper_edges.size() * V * V : 0.0;
    }
    
    virtual void InsertEdge(int s, int t);
    virtual void DeleteEdge(int s, int t);
    virtual void InsertNode(int v);
    virtual void DeleteNode(int v);
    
    void SetTradeOffParam(int x) { tradeoff_param = x;}
    friend class HyperEdge;
  };
};

#endif /* DYNAMIC_CENTRALITY_HAY_H */


