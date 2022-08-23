#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#include "../util/configuration.h"
#include "../util/insertion_mgr.h"

/**
 * Run the parallel experiments
 * Both the group_size and num_groups are modified by these experiments
 */
int main(int argc, char** argv) {
  if (argc != 5) {
    std::cout << "Incorrect number of arguments. "
                 "Expected three but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream, output_file, csv_out_file, max_threads" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string input  = argv[1];
  std::string output = argv[2];
  std::string csv_file = argv[3];
  int max_thr = std::stoi(argv[4]);
  if (max_thr <= 0 || max_thr > 100) {
    printf("Argument CPU cores invalid. Out of range [1,100");
    exit(EXIT_FAILURE);
  }
  // max_thr for group size experiment needs to be divisible by 20
  int gr_max_thr = max_thr - (max_thr % 20);

  std::vector<int> sizes{1,2,4,10,20};

  // Group Size Experiment
  printf("Running group_size experiment\n");
  for (int size : sizes) {
    if (size > gr_max_thr) break;

    // for this experiment we set group_size and num_groups
    sys_config conf;
    conf.group_size = size;
    conf.num_groups = gr_max_thr / size;
    perform_insertions(input, output + "_group_exp_" + std::to_string(size), conf);
  }


  // Num Threads Experiment
  printf("Running num_threads experiment\n");
  const int group_size = 1;
  const int inc = 4;

  std::vector<std::pair<int,double>> ingestion_rates;
  for (int groups = 1; groups <= max_thr + (max_thr % inc); groups += inc) {
    if (groups > max_thr) groups = max_thr;

    // for this experiment we set group_size and num_groups
    sys_config conf;
    conf.group_size = group_size;
    conf.num_groups = groups;
    ingestion_rates.push_back({
      groups,
      perform_insertions(input, output + "_threads_exp_" + std::to_string(groups), conf, 60).first.first
    });

    if (groups == 1) groups = 0; // correct shifted thread numbers because starting at 1
  }

  std::ofstream csv {csv_file};
  csv << "threads,insertion_rate" << std::endl;
  for (auto p : ingestion_rates) {
    csv << p.first << "," << p.second << std::endl;
  }
  csv.close();
}
