#include <fstream>
#include <unordered_map>
#include <iostream>

#include <graph.h>
#include <vector>
#include <mat_graph_verifier.h>

void test_continuous(std::string input_file, unsigned samples) {
  // create input stream

  std::ifstream in {input_file};
  node_id_t n;
  uint64_t  m;

  in >> n >> m;

  Graph g{n};

  size_t total_edges = static_cast<size_t>(n - 1) * n / 2;
  size_t updates_per_sample = m / samples;
//  std::vector<bool> adj(total_edges);
  unsigned long num_failure = 0;
  std::vector<double> flush_times (samples, 0.0);
  std::vector<double> cc_times (samples, 0.0);
  std::vector<double> cc_times_with_return (samples, 0.0);
  std::vector<double> tot_times (samples, 0.0);

  auto empty_start = std::chrono::steady_clock::now();
  auto empty_res = g.connected_components(true);
  double empty_time = std::chrono::duration<double>(std::chrono::steady_clock
        ::now() - empty_start).count();
  auto empty_flush = std::chrono::duration<double>(g.cc_flush_end_time - g.cc_flush_start_time).count();
  auto empty_cc = std::chrono::duration<double>(g.cc_end_time - g.cc_start_time).count();
  std::cout << "Empty graph\nNumber CCs: " << empty_res.size() << "\nTotal time: "
    << empty_time << "\nFLush time: " << empty_flush << "\nCC time: "
    << empty_cc << std::endl;

  node_id_t t,a,b;
  for (unsigned long i = 0; i < samples; i++) {
    std::cout << "Starting updates" << std::endl;
    for (unsigned long j = 0; j < updates_per_sample; j++) {
      in >> t >> a >> b;
//      GraphUpdate upd = {{a,b},t};
//      uint64_t edge_uid = MatGraphVerifier::get_uid(upd.first.first, upd.first.second);
      g.update({{a,b},INSERT});
//      adj[edge_uid] = !adj[edge_uid];
    }
    try {
//      g.set_verifier(std::make_unique<MatGraphVerifier>(n, adj));
      std::cout << "Running cc" << std::endl;
      auto start = std::chrono::steady_clock::now();
      auto res = g.connected_components(true);
      auto end = std::chrono::steady_clock::now();
      std::cout << "Number CCs: " << res.size();
      cc_times_with_return[i] = std::chrono::duration<double>
            (end - g.cc_start_time).count();
      flush_times[i] = std::chrono::duration<double>(g.cc_flush_end_time -
            g.cc_flush_start_time).count();
      cc_times[i] = std::chrono::duration<double>(g.cc_end_time -
            g.cc_start_time).count();
      tot_times[i] = std::chrono::duration<double>(end - start).count();
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
  std::cout << "Flush timings\n";
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << flush_times[i] << " sec\n";
  }
  std::cout << "\n";
  std::cout << "CC timings\n";
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << cc_times[i] << " sec\n";
  }
  std::cout << "\n";
  std::cout << "CC timings with return\n";
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << cc_times_with_return[i] << " sec\n";
  }
  std::cout << "\n";
  std::cout << "Total processing time\n";
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << tot_times[i] << " sec\n";
  }
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
