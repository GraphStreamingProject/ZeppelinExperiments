#include <fstream>
#include <string>
#include <chrono>
#include <sstream>
#include <thread>
#include <iostream>
#include <unistd.h>

#include <graph.h>
#include "configuration.h"
#include "insertion_mgr.h"

static bool shutdown = false;

/*
 * Function which is run in a seperate thread and will query
 * the graph for the number of updates it has processed
 * the thread writes that information to a given file
 * @param total       the total number of edge updates
 * @param g           the graph object to query
 * @param start_time  the time that we started stream ingestion
 */
void track_insertions(std::string output_file, uint64_t total, Graph *g, std::chrono::steady_clock::time_point start_time) {
  total = total * 2;                // we insert 2 edge updates per edge
  std::ofstream out{output_file}; // open the outfile
  if (!out.is_open()) {
    printf("ERROR:Could not open output file! %s\n", output_file.c_str());
    exit(EXIT_FAILURE);
  }

  printf("Insertions\n");
  printf("Progress:                    | 0%%\r"); fflush(stdout);
  std::chrono::steady_clock::time_point start = start_time;
  std::chrono::steady_clock::time_point prev  = start_time;
  uint64_t prev_updates = 0;
  int percent = 0;

  while(true) {
    sleep(5);
    uint64_t updates = g->num_updates;
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::duration<double> total_diff = now - start;
    std::chrono::duration<double> cur_diff   = now - prev;

    // calculate the insertion rate and write to file
    uint64_t upd_delta = updates - prev_updates;
    // divide insertions per second by 2 because each edge is split into two updates
    // we care about edges per second not about stream updates
    int ins_per_sec = (((float)(upd_delta)) / cur_diff.count()) / 2;

    int amount = upd_delta / (total * .01);
    if (amount > 1) {
      percent += amount;
      out << percent << "% :\n";
      out << "Updates per second sinces last entry: " << ins_per_sec << "\n";
      out << "Time since last entry: " << (int) cur_diff.count() << "\n";
      out << "Total runtime so far: " << (int) total_diff.count() << "\n\n";

      prev_updates += upd_delta;
      prev = now; // reset start time to right after query
    }
    
    if (updates >= total || shutdown)
      break;

    // display the progress
    int progress = updates / (total * .05);
    printf("Progress:%s%s", std::string(progress, '=').c_str(), std::string(20 - progress, ' ').c_str());
    printf("| %i%% -- %i per second\r", progress * 5, ins_per_sec); fflush(stdout);
  }
  printf("Progress:====================| Done      \n");
  out.close();
  return;
}

void perform_insertions(std::string binary_input, std::string output_file, sys_config config, long timeout) {
  // create the structure which will perform buffered input for us
  BinaryGraphStream stream(binary_input, 32 * 1024);
  shutdown = false;

  // write the configuration to the config files
  write_configuration(config);

  node_id_t num_nodes = stream.nodes();
  long m         = stream.edges();
  long total     = m;
  Graph g{num_nodes};

  auto start = std::chrono::steady_clock::now();
  std::thread querier(track_insertions, output_file, total, &g, start);
  
  if (timeout <= 0) {
    // then just run the whole stream
    while (m--) {
      g.update(stream.get_edge());
    }
  } else {
    // implement a timeout and stop the stream prematurely if necessary
    std::cout << "insertion_mgr: Using a TIMEOUT of " << timeout << " minutes" << std::endl;
    while (m > 0) {
      long i = 0;
      for (; i < m && i < 10000000; i++) {
        g.update(stream.get_edge());
      }
      auto now = std::chrono::steady_clock::now();
      if (std::chrono::duration<double, std::ratio<60>>(now - start).count() > timeout) {
        total = total - m; // reduce total by number of unprocessed edges
	std::cout << "Stream stopped short because of timeout after " << total << " insertions" << std::endl;
	break;
      }
      m -= i;
    }
  }

  std::cout << "Starting CC" << std::endl;

  uint64_t num_CC = g.connected_components().size();
  
  shutdown = true;
  querier.join();
  long double time_taken = static_cast<std::chrono::duration<long double>>(g
        .cc_flush_end_time - start).count();
  long double CC_time = static_cast<std::chrono::duration<long double>>(end -
        g.cc_flush_end_time).count();

  std::ofstream out{output_file,  std::ofstream::out | std::ofstream::app}; //
  // open the outfile
  std::cout << "Number of connected components is " << num_CC << std::endl;
  std::cout << "Writing runtime stats to " << output_file << std::endl;

  std::chrono::duration<double> runtime  = g.cc_flush_end_time - start;

  // calculate the insertion rate and write to file
  // insertion rate measured in stream updates 
  // (not in the two sketch updates we process per stream update)
  float ins_per_sec = (((float)(total)) / runtime.count());
  out << "Procesing " << total << " updates took " << runtime.count() << " seconds, " << ins_per_sec << " per second\n";

  out << "Connected Components algorithm took " << CC_time.count() << " and found " << num_CC << " CC\n";
  out.close();
}

void perform_continuous_insertions(BinaryGraphStream& stream, sys_config config) {
  // write the configuration to the config files
  write_configuration(config);

  node_id_t num_nodes = stream.nodes();
  uint64_t m = stream.edges();
  uint64_t total = m;
  Graph g{num_nodes};

  const int samples = 10;
  const uint64_t upds_per_sample = m / samples;
  unsigned long num_failure = 0;
  std::vector<double> flush_times (samples, 0.0);
  std::vector<double> backup_times (samples, 0.0);
  std::vector<double> cc_alg_times (samples, 0.0);
  std::vector<double> cc_tot_times (samples, 0.0);
  std::vector<double> tot_times (samples, 0.0);
  std::vector<double> insertion_times (samples, 0.0);

  for (int i = 0; i < samples; ++i) {
    auto start = std::chrono::steady_clock::now();
    for (unsigned j = 0; j < upds_per_sample; ++j) {
      g.update(stream.get_edge());
    }
    try {
      std::cout << "Running cc" << std::endl;
      auto res = g.connected_components(true);
      auto end = std::chrono::steady_clock::now();
      std::cout << "Number CCs: " << res.size() << std::endl;
      flush_times[i] = std::chrono::duration<double>(g.flush_return - g.flush_call).count();
      backup_times[i] = std::chrono::duration<double>(g.create_backup_end - g.create_backup_start).count();
      backup_times[i] += std::chrono::duration<double>(g.restore_backup_end - g.restore_backup_start).count();
      cc_alg_times[i] = std::chrono::duration<double>(g.cc_alg_end - g.cc_alg_start).count();
      cc_tot_times[i] = std::chrono::duration<double>(end - cc_start).count();
      tot_times[i] = std::chrono::duration<double>(end - start).count();
      insertion_times[i] = std::chrono::duration<double>(g.flush_return - start).count();
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
  double cum_ingestion_time = 0.0;
  for (unsigned i = 0; i < samples; ++i) {
    std::cout << i << ": " << insertion_times[i] << " sec\n";
    cum_ingestion_time += insertion_times[i];
  }
  std::cout << "\nIngestion rate: " << (double) total / cum_ingestion_time <<
  " sec" << std::endl;
}
