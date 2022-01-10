#include <fstream>
#include <unordered_map>
#include <iostream>

#include <graph.h>
#include <mat_graph_verifier.h>
#include <binary_graph_stream.h>

void test_continuous(std::string input_file, unsigned samples) {
  // create input stream
  BinaryGraphStream stream(input_file, 32 * 1024);

  node_id_t n = stream.nodes();
  uint64_t  m = stream.edges();

  Graph g{n};

  size_t total_edges = static_cast<size_t>(n - 1) * n / 2;
  node_id_t updates_per_sample = m / samples;
  std::vector<bool> adj(total_edges);
  unsigned long num_failure = 0;

  for (unsigned long i = 0; i < samples; i++) {
    std::cout << "Starting updates" << std::endl;
    for (unsigned long j = 0; j < updates_per_sample; j++) {
      GraphUpdate upd = stream.get_edge();
      uint64_t edge_uid = MatGraphVerifier::get_uid(upd.first.first, upd.first.second);
      g.update(upd);
      adj[edge_uid] = !adj[edge_uid];
    }
    try {
      g.set_verifier(std::make_unique<MatGraphVerifier>(n, adj));
      std::cout << "Running cc" << std::endl;
      g.connected_components(true);
    } catch (const OutOfQueriesException& e) {
      num_failure++;
      std::cout << "CC #" << i << "failed with NoMoreQueries" << std::endl;
    } catch (const NotCCException& e) {
      num_failure++;
      std::cout << "CC #" << i << "failed with NotCC" << std::endl;
    } catch (const BadEdgeException& e) {
      num_failure++;
      std::cout << "CC #" << i << "failed with BadEdge" << std::endl;
    }
  }
  std::clog << n << ',' << num_failure << std::endl;
}

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "Incorrect number of arguments. "
                 "Expected three but got " << argc-1 << std::endl;
    std::cout << "Arguments are: input_stream, samples, runs" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string input_file = argv[1];
  unsigned samples = std::stoi(argv[2]);
  unsigned runs = std::stoi(argv[3]);

  for (unsigned i = 0; i < runs; i++) {
    std::cout << "run: " << i << std::endl;
    test_continuous(input_file, samples);
  }
}
