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
  if (argc != 4) {
    std::cout << "Incorrect number of arguments. "
                 "Expected two but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream, output_file, csv_output_file" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string input       = argv[1];
  std::string output      = argv[2];
  std::string csv_outfile = argv[3];

  // create a binary stream to get the number of nodes in the graph
  node_id_t num_nodes;
  {
    BinaryGraphStream stream(input, 1024);
    num_nodes = stream.nodes();
  }

  // Calculate the number of updates per sketch sized buffer
  int updates_in_sketch = 42 * pow(log2(num_nodes), 2) / (log2(3) - 1); 

  std::vector<int> sizes{2, 1, -2, -3, -4, -6, -10};

  // assert that we also check 1000, 500, 250, and 1 updates per buffer
  std::vector<int> upds_sizes{1000, 500, 250, 1};

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
  for (int in_mem = 0; in_mem < 2; ++in_mem) {
    for (int size : sizes) {
      printf("Running experiment, buffer size factor %i\n", size);

      sys_config conf;
      conf.gutter_factor = size;
      conf.use_tree = !in_mem;
      auto ingestion_res = perform_insertions(input, output + "_" + std::to_string(size), conf, 60);
      csv_res.push_back({in_mem, {
        ((size > 0) ? updates_in_sketch * size : -updates_in_sketch / size),
        ingestion_res.first.first
      }});
    }
  }

  std::ofstream csv {csv_outfile};
  csv << "mem,buffer_size,percent_buffer_size,ingestion_rate" << "\n";
  std::sort(csv_res.begin(), csv_res.end());
  for (const auto& p : csv_res) {
    csv << (p.first ? "yes" : "no") << p.second.first << ","
        << (double)p.second.first / updates_in_sketch << "," << p.second.second << "\n";
  }
}
