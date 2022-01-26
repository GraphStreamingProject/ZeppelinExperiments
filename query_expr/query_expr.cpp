#include <string>
#include <iostream>

#include "../util/configuration.h"
#include "../util/insertion_mgr.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Incorrect number of arguments. "
                 "Expected two but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream, output_file" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string input  = argv[1];
  std::string output = argv[2];

  // TODO: change standalone buffer size to ~100 elems
  // so an empty sys_config will suffice
  sys_config conf;

  backup_configuration();
  perform_continuous_insertions(input, output, conf);
  restore_configuration();
}
