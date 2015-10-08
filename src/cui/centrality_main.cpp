#include "algorithm/centrality.hpp"
#include "algorithm/dynamic_centrality_bms.hpp"
#include "algorithm/dynamic_centrality_hay.hpp"
#include "gflags/gflags.h"
#include <iostream>
using namespace std;
using namespace betweenness_centrality;

DEFINE_string(graph_file, "-", "input graph file.");
DEFINE_string(query_file, "-", "input query file.");
DEFINE_string(algorithm, "naive", "naive, sample, bms, or hay");
DEFINE_int32(num_samples, 1000, "the number of samples used to estimate centrality values.");


CentralityBase *GetAlgorithmFromName(const string &algo_name){
  if (algo_name == "naive"){
    return new CentralityNaive();
  } else if (algo_name == "sample"){
    return new CentralitySample();
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
  FILE *fp = fopen(graph_file.c_str(), "r");
  
  if (fp == NULL){
    cerr << "Cannot open graph_file correctly." << endl;
    exit(EXIT_FAILURE);
  }
  
  es.clear();

  for (int u, v; fscanf(fp, "%d %d", &u, &v) == 2; ){
    es.emplace_back(u, v);
  }
  fclose(fp);
}


void ReadQuery(const string &query_file, vector<int> &qs){
  FILE *fp = fopen(query_file.c_str(), "r");
  
  if (fp == NULL){
    cerr << "Cannot open graph_file correctly." << endl;
    exit(EXIT_FAILURE);
  }
  
  qs.clear();

  for (int q; fscanf(fp, "%d", &q) == 1; ){
    qs.push_back(q);
  }
  fclose(fp);
}

int main(int argc, char *argv[])
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  
  CentralityBase *cb = GetAlgorithmFromName(FLAGS_algorithm);
  
  vector<pair<int, int> > es;
  ReadGraph(FLAGS_graph_file, es);
  cb->PreCompute(es, FLAGS_num_samples);

  vector<int> qs;
  ReadQuery(FLAGS_query_file, qs);

  for (int q : qs){
    cout << cb->QueryCentrality(q) << endl;
  }
  return 0;
}


