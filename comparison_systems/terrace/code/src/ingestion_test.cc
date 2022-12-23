#include <fstream>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>

#include "BufferedEdgeInput.h"
#include "../include/integerSort/blockRadixSort/blockRadixSort.h"
#include "../include/graph.h"
#include "../include/util.h"
#include "../include/Components.h"

int main (int argc, char * argv[])
{ 
  if (argc != 6)
  {
    std::cout << "Incorrect number of arguments!" << std::endl;
    std::cout << "Arguments are: stream_file, batch_size, file_buffer_size, timeout_hrs, output_file" << std::endl;
    exit(EXIT_FAILURE);
  }
  unsigned long update_buffer_size;
  sscanf(argv[3], "%lu", &update_buffer_size);

  BufferedEdgeInput buff_in{argv[1], update_buffer_size};

  std::ofstream out_file{argv[5]};
  if(!out_file) {
    std::cout << "ERROR: Could not open output file!" << std::endl;
    exit(EXIT_FAILURE);
  }

  size_t timeout_sec = 3600 * std::atoi(argv[4]);

  if (timeout_sec < 3600 || timeout_sec > 24 * 3600) {
    std::cout << "ERROR: timeout out of range! [1,24] hours." << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Setting timeout at " << timeout_sec << " seconds" << std::endl;

  unsigned long num_nodes = buff_in.num_nodes;
  unsigned long num_updates = buff_in.num_edges;

  Graph g(num_nodes);

  unsigned long update_batch_size;
  sscanf(argv[2], "%lu", &update_batch_size);
        
  std::vector<uint32_t> ins_srcs(update_batch_size);
        std::vector<uint32_t> ins_dests(update_batch_size);
  pair_uint *edges = 
    (pair_uint*)calloc(update_batch_size, sizeof(pair_uint));

  using namespace std::chrono;

  uint32_t u, v;
  bool is_delete;
  std::tuple<uint32_t, uint32_t, uint8_t> update;
  unsigned long insert_buffer_count = 0;
  unsigned long log_count = 0;  
  unsigned long hundredth = round (num_updates / 100);
  double time_so_far_secs = 0;
  auto start_time = steady_clock::now();
  auto prev_log_time = start_time;
  auto last_print_time = start_time;
  std::cout << "Percent\tSeconds\tIns/sec" << std::endl;

  size_t status_idx = 0;
  const size_t status_interval = 100000;
  for (unsigned long i = 0; i < num_updates; i++)
  {
    if (++status_idx >= status_interval) {
      // print status and check for timeout
      auto now = steady_clock::now();
      double secs_so_far = duration<double>(now - start_time).count();
      double secs_since_print = duration<double>(now - last_print_time).count();
      double ingestion_rate = status_interval / secs_since_print;
      last_print_time = now;

      std::cout << " " << round(((double)i) / num_updates * 100) 
                << "%\t" << round(secs_so_far) 
                << "\t" << round(ingestion_rate) << "                \r";
      fflush(stdout);

      if (secs_so_far >= timeout_sec) {
        std::cout << std::endl;
        std::cout << "TIMEOUT: exiting early after: " << i << " insertions" << std::endl;
        num_updates = i;
        break;
      }
      status_idx = 0;
    }

    if (log_count >= hundredth)
    {
      auto new_log_time = steady_clock::now();
      
      auto log_interval_secs = (duration<double, std::ratio<1, 1>>(new_log_time - prev_log_time)).count();
            auto updates_per_second = log_count / log_interval_secs;
      time_so_far_secs += log_interval_secs;
      log_count = 0;

      prev_log_time = steady_clock::now();
    }
    else log_count++;

    buff_in.get_edge(update); 
    is_delete = std::get<2>(update) == 1;
    u = std::get<0>(update);
    v = std::get<1>(update);

    if (!is_delete)
    {
      edges[insert_buffer_count].x = u;
      edges[insert_buffer_count].y = v;

      if (insert_buffer_count == update_batch_size-1)
      {

        auto perm = get_random_permutation(
            update_batch_size);
        
        integerSort_y((pair_els *)edges, 
            update_batch_size,
            num_nodes);
        integerSort_x((pair_els *)edges, 
            update_batch_size,
            num_nodes);
        
        for(int j = 0; j < update_batch_size; j++)
        {
          ins_srcs[j] = edges[j].x;
          ins_dests[j] = edges[j].y;
        }

        g.add_edge_batch(ins_srcs.data(), 
          ins_dests.data(), 
          update_batch_size, perm);

        insert_buffer_count = 0;
      }
      else
        insert_buffer_count++;
    }
    else 
    {
      g.remove_edge(u, v);
    }
  }

  // Flush remainder of buffer contents
  if (insert_buffer_count > 0)
  {
    auto perm = get_random_permutation(insert_buffer_count);
    g.add_edge_batch(ins_srcs.data(), ins_dests.data(), 
      insert_buffer_count, perm);
  }

  auto ingest_end_time = steady_clock::now();

  auto CC_start_time = steady_clock::now();
  CC(g);
  auto CC_end_time = steady_clock::now();

  free(edges);

  auto ingest_time_secs = (duration<double, std::ratio<1, 1>>(
    ingest_end_time - start_time)).count();
  auto updates_per_second = num_updates / ingest_time_secs;
  
  auto CC_time_secs = (duration<double, std::ratio<1, 1>>(
      CC_end_time - CC_start_time)).count();

  out_file << updates_per_second  << " " << CC_time_secs << " " << num_nodes << std::endl;
  exit(EXIT_SUCCESS);
}
