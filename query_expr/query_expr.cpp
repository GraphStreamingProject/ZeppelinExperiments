#include <string>
#include <iostream>
#include <cmath>

#include "../util/configuration.h"
#include "../util/insertion_mgr.h"
#include <graph_zeppelin_common.h>

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Incorrect number of arguments. "
                 "Expected one but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream num_buffer_elems" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string input  = argv[1];
  int num_buffer_elems = std::stoi(argv[2]);

  const node_id_t num_nodes = 131072;
  // TODO: change standalone buffer size to ~100 elems
  double unrounded_gf = (42 * pow(log2(num_nodes), 2) /
        (log2(3) - 1)) / num_buffer_elems;
  std::cout << "Unrounded gutter factor: " << unrounded_gf << std::endl;
  int gutter_factor = -1*((int) unrounded_gf);
  // so an empty sys_config will suffice
  sys_config conf {0, 0, gutter_factor};
//  sys_config conf;
  
  
  backup_configuration();
  perform_continuous_insertions(input, conf);
  restore_configuration();
}
