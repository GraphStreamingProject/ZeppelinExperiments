#include <fstream>
#include <iostream>

#include <graph_zeppelin_common.h>
#include "../util/shell_exec.h"

std::string cat(std::string filename) {
  return shell_exec("cat " + filename);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Correct usage is\n"
      "\t/path/to/speed_experiment csv_output_file input_file [input files+]" << std::endl;
    return 1;
  }

  std::string arg_str = argv[2];
  for (int i=3; i<argc; ++i) {
    arg_str += " ";
    arg_str += argv[i];
  }
  std::string csv_out = argv[1];
  std::ofstream csv { csv_out };

  const std::string fname = __FILE__;
  size_t pos = fname.find_last_of("\\/");
  const std::string curr_dir = (std::string::npos == pos) ? "" : fname.substr(0, pos);
  
  // run unlim exprs
  std::cout << shell_exec(curr_dir + "/run_unlim_experiments.sh " + arg_str) << std::endl;
  // now process them
  csv << "alg,nodes,in_ram,max_space,ingestion_rate,cc_time\n";
  for (int i=3; i<argc; ++i) {
    double ins_sec;
    double cc_time;
    node_id_t num_nodes;
    double res;
    double swap;

    auto in_str = cat("unlim_results/test_" + std::string(argv[i]) + "/runtime_stats");
    sscanf(in_str.c_str(), "%lf %lf %d", &ins_sec, &cc_time, &num_nodes);
    in_str = cat("unlim_results/test_" + std::string(argv[i]) + "/max_memory");
    sscanf(in_str.c_str(), "VIRT %lf\nRES  %lf\nSWAP %lf", &res, &res, &swap);

    csv << "gz," << num_nodes << ",yes," << res + swap << ","
        << ins_sec << "," << cc_time << std::endl;
  }

  // run 16GB exprs
  std::cout << shell_exec(curr_dir + "/run_experiments.sh " + arg_str) << std::endl;
  // now process them
  for (int i=3; i<argc; ++i) {
    double ins_sec;
    double cc_time;
    node_id_t num_nodes;
    double res;
    double swap;

    auto in_str = cat("speed_results/test_" + std::string(argv[i]) + "/runtime_stats");
    sscanf(in_str.c_str(), "%lf %lf %d", &ins_sec, &cc_time, &num_nodes);
    in_str = cat("speed_results/test_" + std::string(argv[i]) + "/max_memory");
    sscanf(in_str.c_str(), "VIRT %lf\nRES  %lf\nSWAP %lf", &res, &res, &swap);

    csv << "gz," << num_nodes << ",no," << res + swap << ","
        << ins_sec << "," << cc_time << std::endl;
  }

  return 0;
}