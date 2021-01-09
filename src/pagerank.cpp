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

const double damping = 0.85;

class pagerank {
  graph& g;
  double threshold;
  bool cur;
  size_t round = 0;

private:
  void initialize() {
    cur = false;
    round = 0;
    for (auto n = g.begin(); n < g.end(); n++) {
      g.get_out_degree(n) = g.edge_end(n) - g.edge_begin(n);
      //std::cout<<"initialize\n";
      g.get_pr(n, cur) = 1.0 / g.size_nodes();
    }
  }

  void reset_next_round() {
    auto num_nodes = g.size_nodes();
    for (auto n = g.begin(); n < g.end(); n++) {
      //std::cout<<"reset_n_R\n";
      g.get_pr(n, !cur) = (1.0 - damping) / (double)num_nodes;
    }
  }

  bool is_converged() {
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

  void swap_round() {
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

  void compute_push() {
    initialize();
    //start_measurement();

	//for (auto n = g.begin(); n < g.end(); n++) {
	//for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
	  	//std::cout<<e<<" ";
	//	std::cout<<"edge_dst["<<e<<"]: "<<edge_dst[e]<<"\n";
	//}
      //}

    int consecutive_accesses = 0;
    do {
      reset_next_round();

      for (auto n = g.begin(); n < g.end(); n++) {
        //std::cout<<"c_p\n";
        double my_contribution = damping * g.get_pr(n, cur) / (double)g.get_out_degree(n);
        for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
          //std::cout<<"c_p2\n";
          g.get_pr(g.get_edge_dst(e), !cur) += my_contribution;
	
        }
      }

      swap_round();
      round += 1;
    } while(!is_converged());

    //stop_measurement();
    scale();
    std::cout << "Converged using " << round << " round(s)." << std::endl;
  }

  void print() {
    for (auto n = g.begin(); n < g.end(); n++) {
      std::cout << n << " ";
      std::cout << std::fixed << std::setprecision(6);
      std::cout<<"print\n";
      std::cout << g.get_pr(n, cur) << std::endl;
    }
  }
};

int main(int argc, char *argv[]) {
  if (argc < 4) {
  std::cerr << "Usage: " << argv[0] << " <input.dimacs> <pull|push> <threshold> [sorted]\n";
    return 0;
  }

  //if(argc < 6)
  // {
//	std::cerr << "Usage: " << argv[0] << " <input.el> <num_nodes> <num_edges> <pull|push> <threshold> [sorted]\n";
//	return 0;
 //  }

  graph g;
  if (!g.construct_from_dimacs(argv)) {
    return 0;
  }
  //if(!g.construct_from_el(argv))
  // {
//	return 0;
 //  }

#if 1
  //g.print_csr();
  //g.print_csr_transposed();
  //g.print_edges();
  //g.print_in_edges();
#endif

  std::string algo = argv[4];
  if ("pull" != algo && "push" != algo) {
    std::cerr << "Unknown algorithm " << algo << std::endl;
    return 0;
  }

//for (auto n = g.begin(); n < g.end(); n++) {
	//for (auto e = g.edge_begin(n); e < g.edge_end(n); e++) {
	  //	std::cout<<g.get_edge_dst(e)<<" ";
//	}
  //    }


  pagerank pr(g, std::stod(argv[5]));
  if ("pull" == algo) {
    pr.compute_pull();
  } else if ("push" == algo) {
    pr.compute_push();
  }

//  pr.print();
  return 0;
}
