#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <cmath>
#include <cstdlib>

#include "graph.h"
//#include "measure.h"
using namespace std;

const double damping = 0.85;

class pagerank {
  graph& g;
  double threshold;
  bool cur;
  size_t round = 0;

private:
  void initialize() { //initializes the pagerank values
    cur = false;
    round = 0;
    for (auto n = g.begin(); n < g.end(); n++) {
      g.get_out_degree(n) = g.edge_end(n) - g.edge_begin(n);
      //std::cout<<"initialize\n";
      g.get_pr(n, cur) = 1.0 / g.size_nodes();
    }
  }

  void reset_next_round() { //changes the pagerank values for next round computation
    auto num_nodes = g.size_nodes();
    for (auto n = g.begin(); n < g.end(); n++) {
      //std::cout<<"reset_n_R\n";
      g.get_pr(n, !cur) = (1.0 - damping) / (double)num_nodes;
    }
  }

  bool is_converged() { //to check whether pagerank algo is converged by some threshold value
    for (auto n = g.begin(); n < g.end(); n++) {
      //std::cout<<"is_Conv\n";
      const auto cur_pr = g.get_pr(n, cur);
      const auto next_pr = g.get_pr(n, !cur);
      if (fabs(next_pr - cur_pr) > threshold) {
        return false;
      }
    }  
    return true;
  }

  void swap_round() { //to swap the pagerank pointer for current and previous pagerank values
    cur = !cur;
  }

  void scale() {
    double sum = 0.0;
    for (auto n = g.begin(); n < g.end(); n++) {
      //std::cout<<"scale\n";
      sum += g.get_pr(n, cur);
    }
    for (auto n = g.begin(); n < g.end(); n++) {
      //std::cout<<"scale2\n";
      g.get_pr(n, cur) /= sum;
    }
  }

public:
  pagerank(graph& g, double th): g(g), threshold(th) { }

  void compute_pull() {
    initialize();
    //start_measurement();

    do {
      for (auto n = g.begin(); n < g.end(); n++) {
        //std::cout<<"pr\n";
        auto& next_pr = g.get_pr(n, !cur);
        next_pr = (1.0 - damping) / (double)g.size_nodes();

        for (auto ie = g.in_edge_begin(n); ie < g.in_edge_end(n); ie++) {
          auto in_dst = g.get_in_edge_dst(ie);
          //std::cout<<"pr2\n";
          next_pr += damping * g.get_pr(in_dst, cur) / (double)g.get_out_degree(in_dst);
        }
      }

      swap_round();
      round += 1;
    } while(!is_converged());

    //stop_measurement();
    scale();
    std::cout << "Converged using " << round << " round(s)." << std::endl;
  }

uint64_t returnLine(uint64_t address) //determines the cache line number of a given address
{
  return ((address/cache_line_size) - (start_address/cache_line_size));
}
void find_cacheLines(){

  uint64_t total_cachelines = 0;

      for (auto n = g.begin(); n < g.end(); n++) {

        set<uint64_t, greater<uint64_t>> s1;

        for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
         uint64_t address = (uint64_t) &g.get_pr(g.get_edge_dst(e), 0);
         uint64_t cache_line = returnLine( address);
         s1.insert(cache_line);
         

        }
        //std::cout<<"Cache lines required to fit n = " << n <<" are "<<std::dec<< s1.size() << std::endl;
        total_cachelines += s1.size();
      }
  std::cout <<"Total cache lines in original case: " << std::dec <<total_cachelines <<std::endl;

}
void generate_mapping(){ //finds out the cache lines in reordered case

  int avg_degree = g.size_edges()/g.size_nodes();
  uint64_t assigned_value = 0;
  unordered_map<int, uint64_t> umap;

  for (auto n = g.begin(); n < g.end(); n++) {
    if(g.get_out_degree(n) > avg_degree){
      for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
        if(umap.find(g.get_edge_dst(e)) == umap.end())
        {
          umap[g.get_edge_dst(e)] = assigned_value;
          assigned_value++;
        }
      }

    }
  }

  for (auto n = g.begin(); n < g.end(); n++) {
    if(g.get_out_degree(n) <= avg_degree){
      for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
        if(umap.find(g.get_edge_dst(e)) == umap.end())
        {
          umap[g.get_edge_dst(e)] = assigned_value;
          assigned_value++;
        }
      }

    }
  }
uint64_t totalCacheLines = 0;
  for (auto n = g.begin(); n < g.end(); n++) {

    set<uint64_t, greater<uint64_t>> s1;

    for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
      uint64_t address = (uint64_t) &g.get_pr(umap[g.get_edge_dst(e)], 0);
      uint64_t cache_line = returnLine( address);
      s1.insert(cache_line);
      

    }
    //std::cout<<"Cache lines required to fit n = " << n <<" are "<<std::dec<< s1.size() << std::endl;
    totalCacheLines += s1.size();
  }

  std::cout << "CSR reodered edge array:" << std::endl;

  for (auto n = g.begin(); n < g.end(); n++)
  {
    for(auto e = g.edge_begin(n); e < g.edge_end(n); e++)
    {
      std::cout <<g.get_edge_dst(e) <<": "<< umap[g.get_edge_dst(e)] << "\n";
    }
  }

  std::cout << "Total cache lines in reordered case: " <<std::dec<< totalCacheLines<<std::endl;

}
  void compute_push() {
    initialize();

  int avg_degree = g.size_edges()/g.size_nodes();
  uint64_t assigned_value = 0;
  unordered_map<int, uint64_t> umap;

  for (auto n = g.begin(); n < g.end(); n++) {
    if(g.get_out_degree(n) > avg_degree){
      for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
        if(umap.find(g.get_edge_dst(e)) == umap.end())
        {
          umap[g.get_edge_dst(e)] = assigned_value;
          assigned_value++;
        }
      }

    }
  }

  for (auto n = g.begin(); n < g.end(); n++) {
    if(g.get_out_degree(n) <= avg_degree){
      for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
        if(umap.find(g.get_edge_dst(e)) == umap.end())
        {
          umap[g.get_edge_dst(e)] = assigned_value;
          assigned_value++;
        }
      }

    }
  }

for (auto n = g.begin(); n < g.end(); n++) {
  if(umap.find(n) == umap.end())
  {
    umap[n] = assigned_value;
    assigned_value++;
  }
}
cout<<"printing unordered map: "<<endl;
for (auto i : umap) {
        cout << i.first << "   " << i.second  
             << endl; 
}
//push based pagerank computation
    int consecutive_accesses = 0;
    do {
      reset_next_round();

      for (auto n = g.begin(); n < g.end(); n++) {
        //std::cout<<"c_p\n";
        double my_contribution = damping * g.get_pr(n, cur) / (double)g.get_out_degree(umap[n]);
        for (auto e = g.edge_begin(umap[n]); e < g.edge_end(umap[n]); e++) {
          //std::cout<<"c_p2\n";
          g.get_pr(umap[g.get_edge_dst(e)], !cur) += my_contribution;
          // std::cout << &g.get_pr(g.get_edge_dst(e), !cur) << "\t";
	
        }
      }

      swap_round();
      round += 1;
    } while(!is_converged());

    //stop_measurement();
    scale();
    std::cout << "Converged using " << round << " round(s)." << std::endl;
  }

  // void compute_push() {    //original compute_push()
  //   initialize();
  //   //start_measurement();

  //   do {
  //     reset_next_round();

  //     for (auto n = g.begin(); n < g.end(); n++) {
  //       double my_contribution = damping * g.get_pr(n, cur) / (double)g.get_out_degree(n);
  //       for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
  //         g.get_pr(g.get_edge_dst(e), !cur) += my_contribution;
  //       }
  //     }

  //     swap_round();
  //     round += 1;
  //   } while(!is_converged());

  //   //stop_measurement();
  //   scale();
  //   std::cout << "Converged using " << round << " round(s)." << std::endl;
  // }

  void print() {
    for (auto n = g.begin(); n < g.end(); n++) {
      std::cout << n << " ";
      std::cout << std::fixed << std::setprecision(6);
      std::cout<<"print\n";
      std::cout << g.get_pr(n, cur) << std::endl;
    }
  }

double calAvgDeg()
{
  int avg_degree = 0;
  uint64_t max_degree = 0;
  for(auto n = g.begin(); n < g.end(); n++)
  {
    if(g.get_out_degree(n) > max_degree){
      max_degree = g.get_out_degree(n);
    }
  }
  int *deg_hist;
  deg_hist = new int[max_degree+1];

  for(int i = 0; i<=max_degree; i++)
  {
    deg_hist[i] = 0;
  }

  for(auto n = g.begin(); n < g.end(); n++)
  {
    deg_hist[g.get_out_degree(n)]++;
  }

  for(int i =0; i<=max_degree; i++)
  {
    avg_degree += (i*deg_hist[i]);
  }
  return (((double) avg_degree)/g.size_nodes());

}

void analyzeGraph() //functio to find out the avg. degree, hot vertices % and edge coverage %
{
  double avg_degree = ((double)g.size_edges())/g.size_nodes();
  std::cout << g.size_edges() << " " << g.size_nodes() << std::endl;
  std::cout << "Avg degree (normal): " << avg_degree << " Avg degree (weighted calculation): " << calAvgDeg() << endl;
  int covered_edges = 0;
  int hot_vert = 0;
  
  for(auto n = g.begin(); n < g.end(); n++)
  {
    if(g.get_out_degree(n) > avg_degree){
      hot_vert++;
      covered_edges += g.get_out_degree(n);
    }
  }

  double hot_vert_per = (((double) hot_vert)/g.size_nodes())*100;
  double edge_cov_per = (((double) covered_edges)/g.size_edges())*100;

  std::cout << "Hot vertices %: " << hot_vert_per <<" Edge coverage: " << edge_cov_per << std::endl;
  
}

};



int main(int argc, char *argv[]) {

  if(argc < 6)
  {
	std::cerr << "Usage: " << argv[0] << " <input.el> <num_nodes> <num_edges> <pull|push> <threshold> [sorted]\n";
	return 0;
  }

  graph g;
  if (!g.construct_from_dimacs(argv)) {
    return 0;
  }


#if 1
  g.print_csr();
  //g.print_csr_transposed();
  //g.print_edges();
  //g.print_in_edges();
#endif

  std::string algo = argv[4];
  if ("pull" != algo && "push" != algo) {
    std::cerr << "Unknown algorithm " << algo << std::endl;
    return 0;
  }

  pagerank pr(g, std::stod(argv[5]));
  if ("pull" == algo) {
    pr.compute_pull();
  } else if ("push" == algo) {
    pr.compute_push();
  }
	g.print_pr();


pr.find_cacheLines(); //separte function call to determine the number of cachelines in the original case
pr.generate_mapping(); //function call to generate the mapping for the reordering 
pr.analyzeGraph();    //call to function to analyse the graph
  return 0;
}
