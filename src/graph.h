#ifndef SP2018_CS377P_GRAPH_H
#define SP2018_CS377P_GRAPH_H

#define SP2018_CS377P_STRUCT_OF_ARRAYS //It specifies a 2D array for page rank as pr[0-1][0-num_nodes]

#include <string>
#include <cassert>
#include <iostream>
#include <iterator>
#include <set>
#include <unordered_map> 

#ifndef SP2018_CS377P_STRUCT_OF_ARRAYS
struct Node {
  double pr[2];
  int out_degree;
};
#endif

extern uint64_t start_address; //to get start address of the pr[][] which forms the property array, to be set inside graph.cpp
extern uint64_t cache_line_size; //in bytes


class graph {
public:
  using node_t = int;
#ifndef SP2018_CS377P_STRUCT_OF_ARRAYS
  using node_data_t = Node;
#endif
  using edge_t = int;
  using edge_data_t = int;
  using in_edge_t = edge_t;
  using in_edge_data_t = edge_data_t;



private:
  node_t num_nodes;
  edge_t num_edges;

#ifndef SP2018_CS377P_STRUCT_OF_ARRAYS
  node_data_t* node;
#else
  double* pr[2];
  int* out_degree;
#endif
//initial commit
  // the csr graph
  edge_t* edge_range; //offset array acc to out-degree
  node_t* edge_dst;   //neighbour array acc to out-degree
  edge_data_t* edge_data; //weight array acc to out-degree


  // the transposed csr graph
  in_edge_t* in_edge_range;   //offset array acc to in-degree
  node_t* in_edge_dst;        //neighbour array acc to in-degree
  in_edge_data_t* in_edge_data;    //weight array acc to in-degree

  bool is_allocated;

public:
  node_t size_nodes() { return num_nodes; }
  edge_t size_edges() { return num_edges; }

  node_t begin() { return 0; }
  node_t end() { return num_nodes; }

#ifndef SP2018_CS377P_STRUCT_OF_ARRAYS
  node_data_t& get_data(node_t n) { assert(n >= 0 && n < num_nodes); return node[n]; }
  double& get_pr(node_t n, bool which) { //std::cout<<"n: "<<n<<"\n"; 
//assert( n >= 0 && n < num_nodes); 
return node[n].pr[which]; }
  int& get_out_degree(node_t n) { assert( n >= 0 && n < num_nodes); return node[n].out_degree; }
#else
  double& get_pr(node_t n, bool which) {
//std::cout<<"n: "<<n<<" which: "<<which<<"\n"; 
assert( n >= 0 && n < num_nodes); return pr[which][n]; }
  int& get_out_degree(node_t n) { assert( n >= 0 && n < num_nodes); return out_degree[n]; }
#endif

  edge_t edge_begin(node_t n) { assert(n >=0 && n < num_nodes); return edge_range[n]; }
  edge_t edge_end(node_t n) { assert(n >= 0 && n < num_nodes); return edge_range[n+1]; }

  node_t get_edge_dst(edge_t e) { assert(e >= 0 && e < num_edges); return edge_dst[e]; }
  edge_data_t& get_edge_data(edge_t e) { assert(e >= 0 && e < num_edges); return edge_data[e]; }

  in_edge_t in_edge_begin(node_t n) { assert(n >= 0 && n < num_nodes); return in_edge_range[n]; }
  in_edge_t in_edge_end(node_t n) { assert(n >= 0 && n < num_nodes); return in_edge_range[n+1]; }

  node_t get_in_edge_dst(in_edge_t ie) { assert(ie >= 0 && ie < num_edges); return in_edge_dst[ie]; }
  in_edge_data_t& get_in_edge_data(in_edge_t ie) { assert(ie >= 0 && ie < num_edges); return in_edge_data[ie]; }

  graph();
  ~graph();
  graph(const graph&) = delete;
  graph& operator=(const graph&) = delete;

  bool construct_from_dimacs(char *argv[]);
  bool construct_from_el(char *argv[]);
  void print_csr();
  void print_csr_transposed();
  void print_edges();
  void print_in_edges();
  void print_pr();

};

#endif
