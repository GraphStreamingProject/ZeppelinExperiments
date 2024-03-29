#include <fstream>
#include <string>
#include <chrono>
#include <sstream>
#include <thread>
#include <iostream>
#include <unistd.h>

#include <graph.h>
#include <binary_graph_stream.h>
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

InsertionData perform_insertions(std::string binary_input, std::string output_file, sys_config config, long timeout) {
  // create the structure which will perform buffered input for us
  BinaryGraphStream stream(binary_input, 32 * 1024);
  shutdown = false;

  auto graph_config = create_graph_config(config);

  node_id_t num_nodes = stream.nodes();
  long m              = stream.edges();
  long total          = m;
  Graph g{num_nodes, graph_config};

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
  std::chrono::duration<double> runtime = g.flush_end - start;
  std::chrono::duration<double> CC_time = g.cc_alg_end - g.cc_alg_start;

  std::ofstream out{output_file,  std::ofstream::out | std::ofstream::app}; // open the outfile
  std::cout << "Number of connected components is " << num_CC << std::endl;
  std::cout << "Writing runtime stats to " << output_file << std::endl;

  // calculate the insertion rate and write to file
  // insertion rate measured in stream updates 
  // (not in the two sketch updates we process per stream update)
  float ins_per_sec = (((float)(total)) / runtime.count());
  out << "Procesing " << total << " updates took " << runtime.count() << " seconds, " << ins_per_sec << " per second\n";

  out << "Connected Components algorithm took " << CC_time.count() << " and found " << num_CC << " CC\n";
  out.close();

  return {ins_per_sec, CC_time.count(), num_nodes};
}
