#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <unordered_map> 

#include "graph.h"
using namespace std;

uint64_t start_address;
uint64_t cache_line_size = 64;

graph::graph() {
  is_allocated = false;
}

graph::~graph() {
  if (!is_allocated) {
    return;
  }

#ifndef SP2018_CS377P_STRUCT_OF_ARRAYS
  delete [] node;
//  std::cout << "array of struct" << std::endl;
#else
  delete [] pr[0];
  delete [] pr[1];
  delete [] out_degree;
//  std::cout << "struct of arrays" << std::endl;
#endif
  delete [] edge_range;
  delete [] edge_dst;
  delete [] edge_data;
  delete [] in_edge_range;
  delete [] in_edge_dst;
  delete [] in_edge_data;
}

struct Edge {
  int src, dst, weight;

public:
  Edge(int s, int d, int w): src(s), dst(d), weight(w) { }
};

void print_coordinates(std::vector<Edge>& v) {
  for (auto e: v) {
    std::cout << e.src << " " << e.dst << " " << e.weight << std::endl;
  }
}

//not in use currently
std::vector<Edge> parse_el(std::fstream& f_dimacs, int& num_nodes, int& num_edges) {
  
  std::string line;
  std::vector<Edge> edges;
  
  //num_nodes = -1;
  //num_edges = 0;

  // parse el
  while (std::getline(f_dimacs, line)) {
    // skip comments
    if ('#' == line[0]) {
      continue;
    }
    
    

    std::vector<std::string> token;
    boost::split(token, line, [] (char c) { return c == '\t'; });

    // p <problem_type> <num_nodes> <num_edges>
    //if ("p" == token[0]) {
      //num_nodes = std::stoi(token[2]);
      //num_edges = std::stoi(token[3]);
    //}

    // a <src> <dst> <weight>
    //else if ("a" == token[0]) {
      edges.push_back(Edge{std::stoi(token[0]), std::stoi(token[1]), 1});
    //}
     
     //if(num_nodes < std::stoi(token[0])-1)
	//{
	//	num_nodes = std::stoi(token[0])-1;
	//}   
     //else if(num_nodes < std::stoi(token[1])-1)   
	//{
	//	num_nodes = std::stoi(token[1])-1;
	//}
	//num_edges++;

  } // end while (getline)

//  print_coordinates(edges);
  return edges;

}

//returns edges after parsing the dimacs format
std::vector<Edge> parse_dimacs(std::fstream& f_dimacs, int& num_nodes, int& num_edges) {
  std::string line;
  std::vector<Edge> edges;

  // parse dimacs
  while (std::getline(f_dimacs, line)) {
    // skip comments
    if ('c' == line[0]) {
      continue;
    }

    std::vector<std::string> token;
    boost::split(token, line, [] (char c) { return c == ' '; });

    // p <problem_type> <num_nodes> <num_edges>
    if ("p" == token[0]) {
      num_nodes = std::stoi(token[2]);
      num_edges = std::stoi(token[3]);
    }

    // a <src> <dst> <weight>
    else if ("a" == token[0]) {
      edges.push_back(Edge{std::stoi(token[1])-1, std::stoi(token[2])-1, std::stoi(token[3])});
    }
  } // end while (getline)

//  print_coordinates(edges);
  return edges;
}

void graph::reorder_edge_dst()
{

    int avg_degree = num_edges / num_nodes;
      uint64_t assigned_value = 0;
    unordered_map<int, uint64_t> umap;

  for (auto n = begin(); n < end(); n++) {
    if(get_out_degree(n) > avg_degree){
      for (auto e = edge_begin(n); e < edge_end(n); e++) {
        if(umap.find(get_edge_dst(e)) == umap.end())
        {
          umap[get_edge_dst(e)] = assigned_value;
          assigned_value++;
        }
      }

    }
  }

  for (auto n = begin(); n < end(); n++) {
    if(get_out_degree(n) <= avg_degree){
      for (auto e = edge_begin(n); e < edge_end(n); e++) {
        if(umap.find(get_edge_dst(e)) == umap.end())
        {
          umap[get_edge_dst(e)] = assigned_value;
          assigned_value++;
        }
      }

    }
  }

  for(auto e = 0; e < num_edges; e++)
  {
    edge_dst[e] = edge_dst[umap[e]];
  }

}

//function: to generate the csr format
void gen_csr(std::vector<Edge>& edges, 
    graph::edge_t* edge_range, graph::node_t* edge_dst, graph::edge_data_t* edge_data, 
    int num_nodes, int num_edges)
{
  std::sort(edges.begin(), edges.end(), 
      [] (const Edge& e1, const Edge& e2) { 
        return (e1.src < e2.src);
      });
  
  int cur_node = -1;
  int cur_edge = 0;
  while (cur_edge < num_edges) {
    const auto& e = edges[cur_edge];
    if (e.src > cur_node) {
      cur_node++;
      edge_range[cur_node] = cur_edge;
    }
    else {
      edge_dst[cur_edge] = e.dst;
      edge_data[cur_edge] = e.weight;
      cur_edge++;
    }
  }

  for (int i = cur_node + 1; i <= num_nodes; i++) {
    edge_range[i] = num_edges;
  }
  edge_dst[num_edges] = num_nodes;
  edge_data[num_edges] = 0;
  

}

void gen_transposed_csr(std::vector<Edge>& edges,
    graph::in_edge_t* in_edge_range, graph::node_t* in_edge_dst, graph::in_edge_data_t* in_edge_data, 
    int num_nodes, int num_edges)
{
  std::sort(edges.begin(), edges.end(), 
      [] (const Edge& e1, const Edge& e2) { 
        return (e1.dst < e2.dst);
      });

  int cur_node = -1;
  int cur_edge = 0;
  while (cur_edge < num_edges) {
    const auto& e = edges[cur_edge];
    if (e.dst > cur_node) {
      cur_node++;
      in_edge_range[cur_node] = cur_edge;
    } else {
      in_edge_dst[cur_edge] = e.src;
      in_edge_data[cur_edge] = e.weight;
      cur_edge++;
    }
  }

  for (int i = cur_node + 1; i <= num_nodes; i++) {
    in_edge_range[i] = num_edges;
  }
  in_edge_dst[num_edges] = num_nodes;
  in_edge_data[num_edges] = 0;
}



bool graph::construct_from_dimacs(char *argv[]) {
  std::fstream f_dimacs(argv[1], std::ios_base::in);
  if (!f_dimacs) {
    std::cerr << "Failed to open " << argv[1] << std::endl;
    return false;
  }

  std::vector<Edge> edges = parse_dimacs(f_dimacs, num_nodes, num_edges);


#ifndef SP2018_CS377P_STRUCT_OF_ARRAYS
  node = new node_data_t [num_nodes+1];
  std::cout<<"size of single node element: "<<(sizeof(node)/sizeof(node[0]))<<"\n";
#else
  pr[0] = new double [num_nodes+1];
  pr[1] = new double [num_nodes+1];
  out_degree = new int [num_nodes+1];
  start_address = (uint64_t) &pr[0][0]; //taking the start address of the pr array
  std::cout<<"start address: "<<std::hex<<start_address<<std::endl;
#endif
  edge_range = new edge_t [num_nodes+1];
  in_edge_range = new in_edge_t [num_nodes+1];

  edge_dst = new node_t [num_edges+1];
  edge_data = new edge_data_t [num_edges+1];

  //edge_data_start_address = edge_data;
  //std::cout<<"size of single edge_data element: "<<(sizeof(edge_data)/sizeof(edge_data[0]))<<"\n";

  in_edge_dst = new node_t [num_edges+1];
  in_edge_data = new in_edge_data_t [num_edges+1];

  is_allocated = true;

  gen_csr(edges, edge_range, edge_dst, edge_data, num_nodes, num_edges);
  gen_transposed_csr(edges, in_edge_range, in_edge_dst, in_edge_data, num_nodes, num_edges);

  reorder_edge_dst();


  return true;
}

bool graph::construct_from_el(char *argv[]) {
  std::fstream f_el(argv[1], std::ios_base::in);
  if (!f_el) {
    std::cerr << "Failed to open " << argv[1] << std::endl;
    return false;
  }

  //std::vector<Edge> edges = parse_dimacs(f_dimacs, num_nodes, num_edges);
  std::vector<Edge> edges = parse_el(f_el, num_nodes, num_edges);
  num_nodes = std::stoi(argv[2]);
  num_edges = std::stoi(argv[3]);
  //std::cout << "Num nodes: "<<num_nodes<<" Num edges: "<<num_edges<<"\n";

#ifndef SP2018_CS377P_STRUCT_OF_ARRAYS
  node = new node_data_t [num_nodes+1];
#else
  pr[0] = new double [num_nodes+1];
  pr[1] = new double [num_nodes+1];
  out_degree = new int [num_nodes+1];
#endif
  edge_range = new edge_t [num_nodes+1];
  in_edge_range = new in_edge_t [num_nodes+1];

  edge_dst = new node_t [num_edges+1];
  edge_data = new edge_data_t [num_edges+1];
  in_edge_dst = new node_t [num_edges+1];
  in_edge_data = new in_edge_data_t [num_edges+1];

  is_allocated = true;

  gen_csr(edges, edge_range, edge_dst, edge_data, num_nodes, num_edges);
 //   for (auto e = 0; e < num_edges; e++) {
 //   std::cout << edge_dst[e] << " ";
 // }
  gen_transposed_csr(edges, in_edge_range, in_edge_dst, in_edge_data, num_nodes, num_edges);

  return true;
}

void graph::print_csr() {
  std::cout << "print csr:" << std::endl;

  for (auto n = 0; n < num_nodes; n++) {
    std::cout << edge_range[n] << " ";
  }
  std::cout << edge_range[num_nodes] << std::endl;

  for (auto e = 0; e < num_edges; e++) {
    std::cout << edge_dst[e] << " ";
  }
  std::cout << edge_dst[num_edges] << std::endl;

  for (auto e = 0; e < num_edges; e++) {
    std::cout << edge_data[e] << " ";
  }
  std::cout << edge_data[num_edges] << std::endl;
}

void graph::print_csr_transposed() {
  std::cout << "print csr (transposed):" << std::endl;

  for (auto n = 0; n < num_nodes; n++) {
    std::cout << in_edge_range[n] << " ";
  }
  std::cout << in_edge_range[num_nodes] << std::endl;

  for (auto ie = 0; ie < num_edges; ie++) {
    std::cout << in_edge_dst[ie] << " ";
  }
  std::cout << in_edge_dst[num_edges] << std::endl;

  for (auto ie = 0; ie < num_edges; ie++) {
    std::cout << in_edge_data[ie] << " ";
  }
  std::cout << in_edge_data[num_edges] << std::endl;
}

void graph::print_edges() {
  std::cout << "edges:" << std::endl;
  for (auto n = 0; n < num_nodes; n++) {
    for (auto e = edge_begin(n); e < edge_end(n); e++) {
      std::cout << n << " " << get_edge_dst(e) << " " << get_edge_data(e) << std::endl;
    }
  }
}

void graph::print_in_edges() {
  std::cout << "in edges:" << std::endl;
  for (auto n = 0; n < num_nodes; n++) {
    for (auto ie = in_edge_begin(n); ie < in_edge_end(n); ie++) {
      std::cout << n << " " << get_in_edge_dst(ie) << " " << get_in_edge_data(ie) << std::endl;
    }
  }
}

void graph::print_pr(){
  std::cout << "pr values" << std::endl;
  for(auto i = 0; i < 2; i++)
  {
    for(auto j = 0; j < num_nodes; j++)
    {
      // if(j==0){
      // double *addr = &pr[i][j];
      std::cout << i <<"\t"<<j<<"\t"<<get_pr(j,i) << "\n";
    // }
    }
    std::cout << std::endl;
  }
}



