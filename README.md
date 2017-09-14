Fully Dynamic Approximate Betweenness Centrality
==============================
This is an implementation of fully dynamic betweenness centrality maintainance method (VLDB '16). This implementation enables you to efficiently maintain approximate betweenness centrality values on large-scale complex networks (social networks, web graphs, co-author networks). If you have interest in our method and experimental results, please see [our paper](http://www.vldb.org/pvldb/vol9/p48-hayashi.pdf).

## Usage
### From Your Program
Please see [src/cui/dynamic_centrality_main.cpp](https://github.com/flowlight0/fully-dynamic-betweenness-centrality/blob/master/src/cui/dynamic_centrality_main.cpp), which is am example usage of our implementation. 

#### Preparation
Include [src/algorithm/dynamic_centrality_hay.hpp](https://github.com/flowlight0/fully-dynamic-betweenness-centrality/blob/master/src/algorithm/dynamic_centrality_hay.hpp) and compile your code with several c++ file in this repository. For example, we can compile [src/cui/dynamic_centrality_main.cpp](https://github.com/flowlight0/fully-dynamic-betweenness-centrality/blob/master/src/cui/dynamic_centrality_main.cpp) with g++ and run this program as follows. From the following example, you will see approximate betweenness centrality of vertices in a sample graph. 

    $ g++ -std=c++11 -Isrc -Ilib src/cui/dynamic_centrality_main.cpp src/algorithm/*.cpp src/common.cpp  -Lbin/lib -lgflags -lpthread -o main
    $ ./main --graph_file=sample/sample.graph --query_file=sample/sample.query --num_samples=10000
    0
    0
    3.945
    3.0025
    0
    0
    1.1264
    9.7216
    13.1328
    12.2368
    10.4256
    6.2336
    0
    0
    0
    3.9175
    2.99
    0
    

#### How to use
* First of all, you need to create an instance of DynamicCentralityHAY (`DynamicCentralityBase *dcb = new DynamicCentralityHAY()`);
* To construct an index, call `dcb->PreCompute(es, num_samples)`. 
    * A type of a variable `es` is `vector<pair<int, int> >`, and each pair `(first, second)` corresponds to an edge from `first` to `second`. A variable   
    * A variable `num_samples` is a parameter that determines an accuracy of this algorithm. Roughly speaking, an absolute error of our betweenness centrality estimation is O((# of vertices)^2 / sqrt(num_samples)). 
* Call `dcb->InsertNode(v)` to add a new vertex v.
* Call `dcb->DeleteNode(v)` to delete an exisiting vertex v. 
* Call `dcb->InsertEdge(u, v)` to add a new edge from u to v.
* Call `dcb->DeleteEdge(u, v)` to delete an existing edge from u to v. 
* Call `dcb->QueryCentrality(v)` to obtain an approximate betweenness centrality of vertex v. 

Curretly, before inserting or deleting edge (u, v), vertices `u` and `v` should be added. If there are not `u` and `v`, our algorithm will cause runtime error.    

### From CUI
Under construction

## Reference 
Takanori Hayashi, Takuya Akiba, and Yuichi Yoshida. [**Fully dynamic betweenness centrality maintenance on massive networks**](http://www.vldb.org/pvldb/vol9/p48-hayashi.pdf).  [*VLDB'16*](http://vldb2016.persistent.com/)

## Contact 
If you have questions or find errors, please contact to me (flowlight0 at gmail.com)
