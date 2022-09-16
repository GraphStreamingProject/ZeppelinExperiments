#include <cassert>
#include <fstream>
#include <iostream>


int main(int argc, char** argv) {
  if (argc != 5) {
    std::cout << "Incorrect number of arguments. "
                 "Expected four but got " << argc-1 << std::endl;
    std::cout << "Arguments are: agm_results cubesketch_results sketch_size sketch_speed" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  std::string agm_infile = argv[1];
  std::string cubesketch_infile = argv[2];

  std::string sketch_size_out = argv[3];
  std::string sketch_speed_out = argv[4];

  std::ifstream agm_in { agm_infile };
  std::ifstream cbs_in { cubesketch_infile };

  if (!agm_in) {
    std::cout << "Could not open agm input file" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (!cbs_in) {
    std::cout << "Could not open cube input file" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ofstream size_out {sketch_size_out};
  std::ofstream speed_out{sketch_speed_out};
  if (!size_out) {
    std::cout << "Could not open sketch size file" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (!speed_out) {
    std::cout << "Could not open sketch speed file" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string agm_line;
  std::string cbs_line;

  size_out << "vectorlength,standard,cubesketch,reduction\n";
  speed_out << "vectorlength,standard,cubesketch,speedup\n";

  long long int vec_len;
  long long int check;
  double agm_ups;
  double cbs_ups;
  long agm_size;
  long cbs_size;
  while (std::getline(agm_in, agm_line)) {
    std::getline(cbs_in, cbs_line);
    std::cout << agm_line << std::endl << cbs_line << std::endl;


    sscanf(agm_line.c_str(), "%lld %lf %ld", &vec_len, &agm_ups, &agm_size);
    sscanf(cbs_line.c_str(), "%lld %lf %ld", &check, &cbs_ups, &cbs_size);
    assert(vec_len == check);
    size_out  << vec_len << "," << agm_size << "," << cbs_size << ","
              << (double)agm_size / (double) cbs_size << "\n";
    speed_out << vec_len << "," << agm_ups << "," << cbs_ups << ","
              << (double) cbs_ups / (double) agm_ups << "\n";
  }
  return 0;
}
