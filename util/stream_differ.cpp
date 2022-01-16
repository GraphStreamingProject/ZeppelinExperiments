#include <iostream>
#include <fstream>

#include <graph.h>
#include <binary_graph_stream.h>

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Program requires 2 arguments:\n"
      "\tstream_differ ascii_filename binary_filename" << std::endl;
      return 1;
  }

  std::ifstream ascii_in {argv[1]};
  BinaryGraphStream bin_in(argv[2], 32 * 1024);
  node_id_t ascii_n, bin_n = stream.nodes();
  edge_id_t ascii_m, bin_m = stream.edges();

  ascii_in >> ascii_n >> ascii_m;

  assert(ascii_n == bin_n);
  assert(ascii_m == bin_m);

  GraphUpdate ascii_upd {{1,1},0};
  GraphUpdate bin_upd;
  
  edge_id_t num_diffs = 0;

  for (int i = 0; i < ascii_m; ++i) {
    ascii_in >> ascii_upd.second >> ascii_upd.first.first >> ascii_upd.first.second;
    bin_upd = bin_in.get_edge();
    if (ascii_upd != bin_upd) {
      ++num_diffs;
      std::cout << "D[" << i << "] "
      "ASCII{{" << ascii_upd.first.first << ", " << ascii_upd.first.second << "}, " << ascii_upd.second << "} "
      "BIN{{" << bin_upd.first.first << ", " << bin_upd.first.second << "}, " << bin_upd.second << "}\n";
    }
    if (num_diffs == 1000) { // temporary stopper
      std::cout << "Too many diffs! Found 1000. Stopping search..." << std::endl;
      return 0;
    }
  }
  std::cout << "Found " << num_diffs << " diffs" << std::endl;
}