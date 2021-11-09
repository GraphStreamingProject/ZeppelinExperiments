#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

#include "../util/configuration.h"
#include "../util/insertion_mgr.h"

/**
 * Run the buffer size experiments
 * The gutter_factor is edited by this experiment
 */
int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "Incorrect number of arguments. "
                 "Expected three but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream, output_file, num_nodes" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string input  = argv[1];
  std::string output = argv[2];
  int num_nodes = std::stoi(argv[3]); // number of bytes it takes to represent a sketch
  if (max_thr <= 0 || max_thr > (1 << 20)) {
    printf("Argument sketch_bytes out of range [1,1MiB]");
    exit(EXIT_FAILURE);
  }

  // backup the current configuration
  backup_configuration();

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
  for (int size : sizes) {
    printf("Running system with buffering bytes = %i\n", size);

    sys_config conf;
    conf.gutter_factor = size;
    perform_insertions(input, output + "/group_exp/" + std::to_string(size), conf);
  }

  // restore the configuration
  restore_configuration();
}
