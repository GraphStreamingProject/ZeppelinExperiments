#include <cassert>
#include <fstream>
#include <iostream>


int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Incorrect number of arguments. "
                 "Expected 2 but got " << argc-1 << std::endl;
    std::cout << "Arguments are: agm_results cubesketch_results" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  std::string agm_infile = argv[1];
  std::string cubesketch_infile = argv[2];

  std::ifstream agm_in { agm_infile };
  std::ifstream cbs_in { cubesketch_infile };

  std::string agm_line;
  std::string cbs_line;

  std::cout << "vectorlength,standard_ups,cubesketch_ups,speedup,"
               "standard_size,cubesketch_size,reduction\n";

  long long int vec_len;
  long long int check;
  long agm_ups;
  long cbs_ups;
  long agm_size;
  long cbs_size;
  while (std::getline(agm_in, agm_line)) {
    assert(std::getline(cbs_in, cbs_line));
    sscanf(agm_line.c_str(), "%lld %d %d", &vec_len, &agm_ups, &agm_size);
    sscanf(cbs_line.c_str(), "%lld %d %d", &check, &cbs_ups, &cbs_size);
    assert(vec_len == check);
    std::cout << vec_len << "," << agm_ups << "," << cbs_ups << ","
              << (double) cbs_ups / (double) agm_ups << "," << agm_size << ","
              << cbs_size << "," << (double) agm_size / (double) cbs_size << "\n";
  }
  return 0;
}