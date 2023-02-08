#include <string>
#include <iostream>
#include <fstream>
#include <thread>

#include "../util/configuration.h"
#include "../util/insertion_mgr.h"
#include "../util/shell_exec.h"

static std::string cat(std::string filename) {
  return shell_exec("cat " + filename, true);
}

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "Incorrect number of arguments. "
                 "Expected three but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream, output_file, use_tree[yes/no]" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string input    = argv[1];
  std::string output   = argv[2];
  std::string tree_str = argv[3];

  std::ofstream out { output };
  if (!out) {
    std::cerr << "ERROR: could not open output file: " << output << std::endl;
    exit(EXIT_FAILURE);
  }

  bool tree = true;
  if (tree_str == "no") tree = false;
  else if (tree_str != "yes") {
    std::cout << "Could not parse: " << tree_str << std::endl;
  }

  // set configuration for this test
  sys_config conf;
  conf.num_groups = std::thread::hardware_concurrency() - 1;
  conf.use_tree = tree;
  if(tree)
    conf.gutter_factor = 1;
  else
    conf.gutter_factor = -2;

  auto res = perform_insertions(input, output + ".log", conf);
  out << res.ingestion_rate << " " << res.cc_time << " " << res.num_nodes << std::endl;

  return 0;
}
