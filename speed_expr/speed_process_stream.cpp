#include <string>
#include <iostream>
#include <fstream>

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

  // speed experiment does not involve changing parameters
  // so an empty sys_config will suffice
  sys_config conf;

  auto res = perform_insertions(input, "./TEMP_DUMP.txt", conf);
  std::ofstream out { output };
  out << res.ingestion_rate << " " << res.cc_time << " " << res.num_nodes << std::endl;

  return 0;
}
