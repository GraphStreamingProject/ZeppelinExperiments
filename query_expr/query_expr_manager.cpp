#include <fstream>
#include <iostream>

#include <graph_zeppelin_common.h>
#include "../util/shell_exec.h"

constexpr size_t GiB = 1024*1024*1024;

std::string cat(std::string filename) {
  return shell_exec("cat " + filename, true);
}

int main(int argc, char** argv) {
  if (argc < 5) {
    std::cerr << "Correct usage is\n"
      "\t/path/to/query_experiment compare_sys[yes/no] csv_output_directory path/to/kron17 timeout" 
      << std::endl;
    return 1;
  }

  // Parse Arguments
  std::string compare_sys = argv[1];
  std::string csv_out_dir = argv[2];
  std::string kron_17_loc = argv[3];
  std::string timeout     = argv[4];
  bool aspen_terrace = false;
  if (compare_sys == "yes") {
    std::cout << "Running GraphZeppelin, Apsen, and Terrace query experiments" << std::endl;
    aspen_terrace = true;

    // assert that aspen/terrace executables are present
    std::ifstream aspen_exec("./aspen_query_expr");
    std::ifstream terrace_exec("./terrace_query_expr");

    if (!aspen_exec || !terrace_exec) {
      std::cout << "Aspen executable or Terrace executable not found!" << std::endl;
      exit(EXIT_FAILURE);
    }
  } else if (compare_sys == "no") {
    std::cout << "Running GraphZeppelin query experiments" << std::endl;
  }
  else {
    std::cout << "Could not parse: " << compare_sys << " expected 'yes' or 'no'" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << std::endl;

  // Open CSV Files
  std::ofstream unlim_query_csv { csv_out_dir + "/unlim_query_expr.csv" , std::fstream::trunc | std::fstream::out};
  if (!unlim_query_csv) {
    std::cout << "Could not open csv output file: " << csv_out_dir + "/unlim_query_expr.csv" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ofstream lim_query_csv { csv_out_dir + "/lim_query_expr.csv" , std::fstream::trunc | std::fstream::out};
  if (!lim_query_csv) {
    std::cout << "Could not open csv output file: " << csv_out_dir + "/lim_query_expr.csv" << std::endl;
    exit(EXIT_FAILURE);
  }

  const std::string fname = __FILE__;
  size_t pos = fname.find_last_of("\\/");
  const std::string curr_dir = (std::string::npos == pos) ? "" : fname.substr(0, pos);

  /******************************************************************
  |                       run unlim query exprs                     |
  |*****************************************************************/
  std::cout << "/-------------------------------------------------\\" << std::endl;
  std::cout << "|   RUNNING QUERY EXPERIMENT: NO MEM LIMIT (4/9)  |" << std::endl;
  std::cout << "\\-------------------------------------------------/" << std::endl;

  shell_exec(curr_dir + "/../build/one_query_expr " + kron_17_loc + " temp_gz.csv yes");
  if (aspen_terrace)
    shell_exec(curr_dir + "/../comparison_systems/experiment_runners/run_compare_unlim_query.sh " + kron_17_loc + " " + timeout);
  
  // now process results
  auto csv_str_gz = cat("temp_gz.csv");
  auto csv_str_asp = cat("temp_asp.csv");
  auto csv_str_ter = cat("temp_ter.csv");

  unlim_query_csv << csv_str_gz << csv_str_asp << csv_str_ter;

  /******************************************************************
  |                       run 16 GiB exprs                          |
  |*****************************************************************/
  std::cout << "/-------------------------------------------------\\" << std::endl;
  std::cout << "|  RUNNING QUERY EXPERIMENT: 12G MEM LIMIT (5/9)  |" << std::endl;
  std::cout << "\\-------------------------------------------------/" << std::endl;
  shell_exec("cgexec -g memory:12_GB " + curr_dir + "/../build/one_query_expr " + kron_17_loc + " temp_gz.csv no");
  if (aspen_terrace)
    shell_exec(curr_dir + "/../comparison_systems/experiment_runners/run_compare_lim_query.sh " + kron_17_loc + " " + timeout);

  // now process results
  csv_str_gz = cat("temp_gz.csv");
  csv_str_asp = cat("temp_asp.csv");

  lim_query_csv << csv_str_gz << csv_str_asp;
  
  return 0;
}
