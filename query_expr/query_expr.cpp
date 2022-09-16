#include <string>
#include <iostream>
#include <cmath>
#include <vector>

#include "../util/configuration.h"
#include "../util/insertion_mgr.h"
#include <graph_zeppelin_common.h>
#include <binary_graph_stream.h>

void perform_continuous_insertions(std::string binary_input, std::string csv_out, sys_config config) {
  BinaryGraphStream stream(binary_input, 32 * 1024);

  auto graph_config = create_graph_config(config);

  node_id_t num_nodes = stream.nodes();
  uint64_t m = stream.edges();
  uint64_t total = m;
  Graph g{num_nodes, graph_config};

  const int samples = 10;
  const uint64_t upds_per_sample = m / samples;
  unsigned long num_failure = 0;
  std::vector<double> flush_times (samples, 0.0);
  std::vector<double> backup_times (samples, 0.0);
  std::vector<double> cc_alg_times (samples, 0.0);
  std::vector<double> cc_tot_times (samples, 0.0);
  std::vector<double> tot_times (samples, 0.0);
  std::vector<double> insertion_times (samples, 0.0);

  // TODO: one insertion/query cycle per sample (for-loop)

  for (int i = 0; i < samples; ++i) {
    std::cout << "new iteration" << std::endl;
    auto start = std::chrono::steady_clock::now();
    for (unsigned j = 0; j < upds_per_sample; ++j) {
      g.update(stream.get_edge());
    }
    try {
      std::cout << "Running cc" << std::endl;
      auto cc_start = std::chrono::steady_clock::now(); 
      auto res = g.connected_components(true);
      auto end = std::chrono::steady_clock::now();

      std::cout << "Number CCs: " << res.size() << std::endl;
      flush_times[i] = std::chrono::duration<double>(g.flush_end - g.flush_start).count();
      cc_alg_times[i] = std::chrono::duration<double>(g.cc_alg_end - g.cc_alg_start).count();
      cc_tot_times[i] = std::chrono::duration<double>(end - cc_start).count();
      tot_times[i] = std::chrono::duration<double>(end - start).count();
      insertion_times[i] = std::chrono::duration<double>(g.flush_end - start).count();

      std::cout << i << ": " << cc_tot_times[i] << ", " << flush_times[i] << ", " << backup_times[i] << ", " << cc_alg_times[i] << ", " << tot_times[i] << ", " << insertion_times[i] << std::endl;

    } catch (const OutOfQueriesException& e) {
      num_failure++;
      std::cout << "CC #" << i << "failed with NoMoreQueries" << std::endl;
    }
  }
  std::clog << "Num failures: " << num_failure << std::endl;
  std::cout << "CC Total timings\n";
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << cc_tot_times[i] << " sec\n";
  }
  std::cout << "\n";

  std::cout << "Flush timings\n";
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << flush_times[i] << " sec\n";
  }
  std::cout << "\n";
  
  std::cout << "CC Algorithm timings\n";
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << cc_alg_times[i] << " sec\n";
  }
  std::cout << "\n";

  std::cout << "Total processing time\n";
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << tot_times[i] << " sec\n";
  }
  std::cout << "\n";
  std::cout << "Ingestion time\n";
  double total_ingestion_time = 0.0;
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << insertion_times[i] << " sec\n";
    total_ingestion_time += insertion_times[i];
  }
  std::cout << "\nIngestion rate: " << (double) total / total_ingestion_time << " sec" << std::endl;

  std::ofstream csv {csv_out};
  csv << "system,percent,flush,cc" << std::endl;
  for (int i = 0; i < samples; ++i) {
    csv << "gz," << 100/samples*(i + 1) << "," << flush_times[i] << "," << cc_alg_times[i] << std::endl;
  }
  csv.close();
  
}

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "Incorrect number of arguments. "
                 "Expected one but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream csv_output_file in_ram[yes/no]" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string input  = argv[1];
  std::string csv_out = argv[2];
  std::string in_ram  = argv[3];
  bool backup_in_ram = false;
  int gutter_factor = -10;

  if (in_ram == "yes") {
    backup_in_ram = true;
    const node_id_t num_nodes = 131072;
    double unrounded_gf = (42 * pow(log2(num_nodes), 2) /
          (log2(3) - 1)) / 100;
    gutter_factor = -1*((int) unrounded_gf);
  }
  else if (in_ram != "no") {
    std::cout << "ERROR: in_ram needs to be 'yes' or 'no'" << std::endl;
    exit(EXIT_FAILURE);
  }

  // sys_config
  sys_config conf {46, 0, gutter_factor, false, backup_in_ram};
  
  perform_continuous_insertions(input, csv_out, conf);
}
