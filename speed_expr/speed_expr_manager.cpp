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
      "\t/path/to/speed_experiment compare_sys[yes/no] timeout_hrs csv_output_directory [input files+]" 
      << std::endl;
    return 1;
  }

  // Parse Arguments
  std::string arg_str = argv[4];
  for (int i=5; i<argc; ++i) {
    arg_str += " ";
    arg_str += argv[i];
  }
  std::string compare_sys = argv[1];
  std::string timeout_hrs = argv[2];
  std::string csv_out_dir = argv[3];

  bool aspen_terrace = false;
  if (compare_sys == "yes") {
    std::cout << "Running GraphZeppelin, Apsen, and Terrace speed experiments" << std::endl;
    aspen_terrace = true;

    // assert that aspen/terrace executables are present
    std::ifstream aspen_exec("./aspen_ingest_expr");
    std::ifstream terrace_exec("./terrace_ingest_expr");

    if (!aspen_exec || !terrace_exec) {
      std::cout << "Aspen executable or Terrace executable not found!" << std::endl;
      exit(EXIT_FAILURE);
    }
  } else if (compare_sys == "no") {
    std::cout << "Running GraphZeppelin speed experiments" << std::endl;
  }
  else {
    std::cout << "Could not parse: " << compare_sys << " expected 'yes' or 'no'" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << std::endl;

  // Open CSV Files
  std::ofstream unlim_speed_csv { csv_out_dir + "/unlim_speed_expr.csv", std::fstream::trunc | std::fstream::out};
  if (!unlim_speed_csv) {
    std::cout << "Could not open csv output file: " << csv_out_dir + "/unlim_speed_expr.csv" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ofstream lim_speed_csv { csv_out_dir + "/lim_speed_expr.csv", std::fstream::trunc | std::fstream::out};
  if (!lim_speed_csv) {
    std::cout << "Could not open csv output file: " << csv_out_dir + "/lim_speed_expr.csv" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ofstream mem_usage_csv {csv_out_dir + "/mem_usage.csv", std::fstream::trunc | std::fstream::out};
  if (!mem_usage_csv) {
    std::cout << "Could not open csv output file: " << csv_out_dir + "/mem_usage.csv" << std::endl;
    exit(EXIT_FAILURE);
  }

  const std::string fname = __FILE__;
  size_t pos = fname.find_last_of("\\/");
  const std::string curr_dir = (std::string::npos == pos) ? "" : fname.substr(0, pos);

  /******************************************************************
  |                       run unlim exprs                           |
  |*****************************************************************/
  std::cout << "/-------------------------------------------------\\" << std::endl;
  std::cout << "|    RUNNING SPEED EXPERIMENT: NO MEMORY LIMIT    |" << std::endl;
  std::cout << "\\-------------------------------------------------/" << std::endl;

  shell_exec(curr_dir + "/run_unlim_speed_expr.sh no " + arg_str);
  shell_exec(curr_dir + "/run_unlim_speed_expr.sh yes " + arg_str);
  if (aspen_terrace)
    shell_exec(curr_dir + "/../comparison_systems/experiment_runners/run_compare_unlim_speed.sh " + timeout_hrs + " " + arg_str);
  
  // now process them
  unlim_speed_csv << "alg,nodes,ingestion_rate\n";
  mem_usage_csv << "alg,nodes,res,swap,disk,total_no_disk\n";
  std::string result_dir;
  std::string system_name;
  for (int s = 0; s < (aspen_terrace? 4 : 2); s++) {
    if (s == 0) {
      result_dir = "unlim_results";
      system_name = "stream_mem";
    }
    else if (s == 1) {
      result_dir = "unlim_gutter_results";
      system_name = "stream_ext";
    }
    else if (s == 2) {
      result_dir = "unlim_results_aspen";
      system_name = "aspen_ulim_e3";
    }
    else {
      result_dir = "unlim_results_terrace";
      system_name = "terrace_lim";
    }
    for (int i=4; i<argc; ++i) {
      std::string test_name = argv[i];
      size_t pos = test_name.find_last_of("\\/");
      test_name = test_name.substr(pos+1);
      std::string result_loc = result_dir + "/test_" + test_name;

      double ins_sec;
      double cc_time;
      node_id_t num_nodes;
      double virt;
      double res;
      double swap;

      auto in_str = cat(result_loc + "/runtime_stats");
      sscanf(in_str.c_str(), "%lf %lf %d", &ins_sec, &cc_time, &num_nodes);
      in_str = cat(result_loc + "/max_memory");
      sscanf(in_str.c_str(), "VIRT %lf\nRES  %lf\nSWAP %lf", &virt, &res, &swap);

      res /= GiB;
      swap = swap == 0? 0 : swap/GiB;

      unlim_speed_csv << system_name << "," << num_nodes << "," << ins_sec << "\n";
      mem_usage_csv << system_name << "," << num_nodes << "," << res << "," << swap << ",0," << res + swap
                    << "," << res+swap << "\n";
    }
  }

  /******************************************************************
  |                       run 16 GiB exprs                          |
  |*****************************************************************/
  std::cout << "/-------------------------------------------------\\" << std::endl;
  std::cout << "|  RUNNING SPEED EXPERIMENT: 16 GiB MEMORY LIMIT  |" << std::endl;
  std::cout << "\\-------------------------------------------------/" << std::endl;
  shell_exec(curr_dir + "/run_lim_speed_expr.sh no " + arg_str);
  shell_exec(curr_dir + "/run_lim_speed_expr.sh yes " + arg_str);
  if (aspen_terrace)
    shell_exec(curr_dir + "/../comparison_systems/experiment_runners/run_compare_lim_speed.sh " + timeout_hrs + " " + arg_str);

  lim_speed_csv << "alg,nodes,ingestion_rate\n";
  // now process them
  for (int s = 0; s < (aspen_terrace? 4 : 2); s++) {
    if (s == 0) {
      result_dir = "speed_results";
      system_name = "stream_mem";
    }
    else if (s == 1) {
      result_dir = "speed_gutter_results";
      system_name = "stream_ext";
    }
    else if (s == 2) {
      result_dir = "speed_results_aspen";
      system_name = "aspen_lim_e3";
    }
    else {
      result_dir = "speed_results_terrace";
      system_name = "terrace_lim";
    }
    for (int i=4; i<argc; ++i) {
      std::string test_name = argv[i];
      size_t pos = test_name.find_last_of("\\/");
      test_name = test_name.substr(pos+1);
      std::string result_loc = result_dir + "/test_" + test_name;

      double ins_sec;
      double cc_time;
      node_id_t num_nodes;
      double virt;
      double res;
      double swap;

      auto in_str = cat(result_loc + "/runtime_stats");
      sscanf(in_str.c_str(), "%lf %lf %d", &ins_sec, &cc_time, &num_nodes);
      in_str = cat(result_loc + "/max_memory");
      sscanf(in_str.c_str(), "VIRT %lf\nRES  %lf\nSWAP %lf", &res, &res, &swap);

      res /= GiB;
      swap = swap == 0? 0 : swap/GiB;

      lim_speed_csv << system_name << "," << num_nodes << "," << ins_sec << "\n";
    }
  }
  return 0;
}
