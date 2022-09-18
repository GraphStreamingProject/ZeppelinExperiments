#include <fstream>
#include <unordered_map>
#include <iostream>
#include <thread>

#include <graph.h>
#include <mat_graph_verifier.h>
#include <binary_graph_stream.h>

unsigned test_continuous(std::string input_file, unsigned samples) {
  // create input stream
  BinaryGraphStream stream(input_file, 32 * 1024);

  node_id_t n = stream.nodes();
  uint64_t  m = stream.edges();

  auto conf = GraphConfiguration().num_groups(std::thread::hardware_concurrency() - 1);

  Graph g{n, conf};
  MatGraphVerifier verify(n);

  size_t total_edges = static_cast<size_t>(n - 1) * n / 2;
  node_id_t updates_per_sample = m / samples;
  std::vector<bool> adj(total_edges);
  unsigned long num_failures = 0;

  for (unsigned long i = 0; i < samples; i++) {
    std::cout << "Starting updates" << std::endl;
    for (unsigned long j = 0; j < updates_per_sample; j++) {
      GraphUpdate upd = stream.get_edge();
      g.update(upd);
      verify.edge_update(upd.first.first, upd.first.second);
    }
    verify.reset_cc_state();
    g.set_verifier(std::make_unique<MatGraphVerifier>(verify));
    std::cout << "Running cc" << std::endl;
    try {
      g.connected_components(true);
    } catch (std::exception& ex) {
      ++num_failures;
      std::cout << ex.what() << std::endl;
    } catch (std::runtime_error& err) {
      ++num_failures;
      std::cout << err.what() << std::endl;
    }
  }
  std::clog << "Sampled " << samples << " times with " << num_failures
      << " failures" << std::endl;
  return num_failures;
}

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cout << "Incorrect number of arguments. "
                 "Expected four but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream, samples, runs, output_file" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string input_file = argv[1];
  unsigned samples = std::stoi(argv[2]);
  unsigned runs = std::stoi(argv[3]);

  unsigned tot_failures = 0;
  for (unsigned i = 0; i < runs; i++) {
    std::cout << "run: " << i << "/" << runs << std::endl;
    tot_failures += test_continuous(input_file, samples);
  }
  std::cout << "Did " << runs << " runs, with " << samples
      << " sample each. Total failures: " << tot_failures << std::endl;

  std::ofstream out {argv[4]};
	std::string stream_name = input_file.substr(input_file.find_last_of("\\/")+1);
  size_t pos = 0;
  while(true) {
    pos = stream_name.find("_", pos);
    if (pos == std::string::npos) break;
    stream_name.replace(pos, 1, "\\_");
		pos += 2;
  }

  out << stream_name << "," << runs*samples << "," << tot_failures << std::endl;
}
