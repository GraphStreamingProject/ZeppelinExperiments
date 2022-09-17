#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

#include "../util/configuration.h"
#include "../util/insertion_mgr.h"
#include <binary_graph_stream.h>

/**
 * Run the buffer size experiments
 * The gutter_factor is edited by this experiment
 */
int main(int argc, char** argv) {
  if (argc != 5) {
    std::cout << "Incorrect number of arguments. "
                 "Expected four but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream, in_mem[yes/no], output_file, csv_output_file" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string input       = argv[1];
  std::string in_mem_str  = argv[2];
  std::string output      = argv[3];
  std::string csv_outfile = argv[4];

  bool in_mem = false;
  if (in_mem_str == "yes") in_mem = true;
  else if (in_mem_str != "no") {
    std::cout << "ERROR: in_mem must be 'yes' or 'no'" << std::endl;
    exit(EXIT_FAILURE);
  }

  // create a binary stream to get the number of nodes in the graph
  node_id_t num_nodes;
  {
    BinaryGraphStream stream(input, 1024);
    num_nodes = stream.nodes();
  }

  // Calculate the number of updates per sketch sized buffer
  int updates_in_sketch = 42 * pow(log2(num_nodes), 2) / (log2(3) - 1);
  std::cout << updates_in_sketch << std::endl;

  std::vector<int> sizes{2, 1, -2, -4, -6, -10};

  // assert that we also check 1000, 250, and 1 updates per buffer
  std::vector<int> upds_sizes{1000, 250, 1};

  for (int updates : upds_sizes) {
    if (updates < updates_in_sketch / 10) {
      sizes.push_back(-1 * updates_in_sketch / updates);
    }
  }

  printf("Running with following buffer size options: ");
  for (int size : sizes) {
    printf("%i ", size);
  }
  printf("\n");

  // Group Size Experiment
  std::vector<std::pair<bool, std::pair<int, double>>> csv_res;
  for (int size : sizes) {
    printf("Running experiment, buffer size factor %i\n", size);

    sys_config conf;
    conf.num_groups = 46;
    conf.gutter_factor = size;
    conf.use_tree = false;
    auto ingestion_res = perform_insertions(input, output + "_" + std::to_string(size), conf, 45);
    csv_res.push_back({in_mem, {
      ((size > 0) ? updates_in_sketch * size : -updates_in_sketch / size),
      ingestion_res.ingestion_rate
    }});
  }

  std::ofstream csv {csv_outfile};
  std::sort(csv_res.begin(), csv_res.end());
  for (const auto& p : csv_res) {
    csv << (p.first ? "yes," : "no,") << p.second.first << ","
        << (double)p.second.first / updates_in_sketch << "," << p.second.second << "\n";
  }
}
