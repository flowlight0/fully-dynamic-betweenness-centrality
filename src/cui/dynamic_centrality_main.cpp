#include "algorithm/dynamic_centrality_hay.hpp"
#include "algorithm/dynamic_centrality_naive.hpp"
#include "algorithm/dynamic_centrality_bms.hpp"
#include "gflags/gflags.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace betweenness_centrality;

DEFINE_string(graph_file, "-", "input graph file.");
DEFINE_string(query_file, "-", "input query file.");
DEFINE_string(algorithm, "hay", "naive, bms, or hay");
DEFINE_int32(num_samples, 1000, "the number of samples used to estimate centrality values.");


DynamicCentralityBase *GetAlgorithmFromName(const string &algo_name){
  if (algo_name == "naive"){
    return new DynamicCentralityNaive();
  } else if (algo_name == "bms"){
    return new DynamicCentralityBMS();
  } else if (algo_name == "hay"){
    return new DynamicCentralityHAY();
  } else {
    cerr << "An algorithm does not exist." << endl;
    exit(EXIT_FAILURE);
  }
}

void ReadGraph(const string &graph_file, vector<pair<int, int> > &es){
  ifstream ifs(graph_file);
  
  if (!ifs.good()){
    cerr << graph_file << ": Cannot open graph_file correctly." << endl;
    exit(EXIT_FAILURE);
  }
  es.clear();

  for (int u, v; ifs >> u >> v; ){
    es.emplace_back(u, v);
  }
  ifs.close();
}

void ProcessQueries(istream &is, DynamicCentralityBase *cb){
  string q;
  while (is >> q){
    int u, v;
    if (q == "Q"){
      is >> v;
      cout << cb->QueryCentrality(v) << endl;
    } else if (q == "VI"){
      is >> v;
      cb->InsertNode(v);
    } else if (q == "VD"){
      is >> v;
      cb->DeleteNode(v);
    } else if (q == "EI"){
      is >> u >> v;
      cb->InsertEdge(u, v);
    } else if (q == "ED"){
      is >> u >> v;
      cb->DeleteEdge(u, v);
    } else {
      getline(is, q);           // dummy
      cerr << "Warning: invalid operation." << endl;
    }
  }
}

int main(int argc, char *argv[])
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  
  DynamicCentralityBase *dcb = GetAlgorithmFromName(FLAGS_algorithm);
  
  vector<pair<int, int> > es;
  ReadGraph(FLAGS_graph_file, es);
  dcb->PreCompute(es, FLAGS_num_samples);

  if (FLAGS_query_file == "-"){
    ProcessQueries(cin, dcb);
  } else {
    ifstream query_is(FLAGS_query_file.c_str());
    if (!query_is.good()){
      cerr << "Cannot open guery_file correctly." << endl;
      exit(EXIT_FAILURE);
    }
    ProcessQueries(query_is, dcb);
  }
  return 0;
}


