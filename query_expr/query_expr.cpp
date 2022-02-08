#include <string>
#include <iostream>
#include <cmath>

#include "../util/configuration.h"
#include "../util/insertion_mgr.h"
#include <binary_graph_stream.h>
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

  // create the structure which will perform buffered input for us
  BinaryGraphStream stream(input, 32 * 1024);
  node_id_t num_nodes = stream.nodes();

  double unrounded_gf = (42 * pow(log2(num_nodes), 2) /
        (log2(3) - 1)) / num_buffer_elems;
  std::cout << "Unrounded gutter factor: " << unrounded_gf << std::endl;
  int gutter_factor = -1*((int) unrounded_gf);
  // so an empty sys_config will suffice
  sys_config conf {0, 0, gutter_factor};

  backup_configuration();
  perform_continuous_insertions(stream, conf);
  restore_configuration();
}
