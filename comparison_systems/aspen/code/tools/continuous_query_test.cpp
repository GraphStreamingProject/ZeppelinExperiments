#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cmath>

#include "../graph/api.h"
#include "../graph/versioned_graph.h"
#include "../algorithms/CC.h"

#include "BufferedEdgeInput.h"

int main (int argc, char * argv[])
{
  if (argc != 7)
  {
    std::cout << "Incorrect number of arguments!" << std::endl;
    std::cout << "Arguments are: stream_file, batch_size, file_buffer_size, timeout_hrs, output_file, num_queries" << std::endl;
    exit(EXIT_FAILURE);
  }

  unsigned long buff_size;
  sscanf(argv[3], "%lu", &buff_size);

  BufferedEdgeInput buff_in{argv[1], buff_size};

  size_t timeout_sec = 3600 * std::atoi(argv[4]);

  if (timeout_sec < 3600 || timeout_sec > 24 * 3600) {
    std::cout << "ERROR: timeout out of range! [1,24] hours." << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Setting timeout at " << timeout_sec << " seconds" << std::endl;

  unsigned long num_nodes = buff_in.num_nodes;
  unsigned long num_updates = buff_in.num_edges;
  
  auto vg = empty_treeplus_graph();
  
  using namespace std::chrono;
  
  unsigned long update_batch_size;
  sscanf(argv[2], "%lu", &update_batch_size);
  typedef std::tuple<uintV, uintV> edge;
  vector<edge> insert_buffer(2 * update_batch_size);
  vector<edge> delete_buffer(2 * update_batch_size);

  unsigned long insert_buffer_count = 0;
  unsigned long delete_buffer_count = 0;

  bool is_delete;
  edge e1, e2;
  std::tuple<uintV, uintV, uint8_t> update;
  unsigned long log_count = 0;
  ofstream query_file{argv[5]};
  int query_percent = 10;

  if (!query_file) {
    std::cout << "ERROR: Could not open output file!" << std::endl;
    exit(EXIT_FAILURE);
  }

  unsigned long query_interval;
  sscanf(argv[6], "%lu", &query_interval);
        query_interval = round(num_updates / query_interval); 

  std::cout << "Percent\tSeconds\tIns/sec" << std::endl;
  auto start_time = steady_clock::now();
  auto last_print_time = start_time;

  size_t status_idx = 0;
  for (unsigned long i = 0; i < num_updates; i++)
  {
    if (++status_idx >= 100000) {
      // print status and check for timeout
      auto now = steady_clock::now();
      double secs_so_far = duration<double>(now - start_time).count();
      double secs_since_print = duration<double>(now - last_print_time).count();
      double ingestion_rate = 1000000 / secs_since_print;
      last_print_time = now;

      std::cout << " " << round(((double)i) / num_updates * 100) 
                << "%\t" << round(secs_so_far) 
                << "\t" << round(ingestion_rate) << "                \r";
      fflush(stdout);

      if (secs_so_far >= timeout_sec) {
        std::cout << "TIMEOUT: exiting early after: " << i << " insertions" << std::endl;
        num_updates = i;
        break;
      }

      status_idx = 0;
    }
    if (log_count >= query_interval)
    {     
      auto CC_start_time = steady_clock::now();
      auto s = vg.acquire_version();
      CC(s.graph);
      auto CC_end_time = steady_clock::now();
      vg.release_version(std::move(s));
      auto CC_time_secs = (duration<double, std::ratio<1, 1>>(
        CC_end_time - CC_start_time)).count();

      query_file << "aspen_top," << query_percent << ",0," << CC_time_secs << "\n";
      query_percent += 10;
      query_file.flush();

      log_count = 0;
    }
    else log_count++;

    buff_in.get_edge(update);
    e1 = std::make_tuple(std::get<0>(update), 
        std::get<1>(update));
    e2 = std::make_tuple(std::get<1>(update), 
        std::get<0>(update));

    is_delete = std::get<2>(update) == 1;

    if (!is_delete)
    {
      insert_buffer[insert_buffer_count++] = e1;
      insert_buffer[insert_buffer_count] = e2;
      
      if (insert_buffer_count == 2*update_batch_size-1)
      {
        std::sort(insert_buffer.begin(), 
            insert_buffer.end());
        vg.insert_edges_batch(2*update_batch_size,
            insert_buffer.data(), false, 
            true, num_nodes, false);
        insert_buffer_count = 0;
      }
      else
        insert_buffer_count++;
    }
    else 
    {
      delete_buffer[delete_buffer_count++] = e1;
      delete_buffer[delete_buffer_count] = e2;

      if (delete_buffer_count == 2*update_batch_size-1)
      {
        std::sort(delete_buffer.begin(), 
            delete_buffer.end());
        vg.delete_edges_batch(2*update_batch_size,
                  delete_buffer.data(), false, 
            true, num_nodes, false);
        delete_buffer_count = 0;
      }
      else
        delete_buffer_count++;
    }
  }

  // Flush remainder of buffer contents
  std::sort(insert_buffer.begin(), 
      insert_buffer.begin() + insert_buffer_count);
  vg.insert_edges_batch(insert_buffer_count, insert_buffer.data(),
      false, true, num_nodes, false);
  std::sort(delete_buffer.begin(), 
      delete_buffer.begin() + insert_buffer_count);
  vg.delete_edges_batch(delete_buffer_count, delete_buffer.data(),
      false, true, num_nodes, false);
  

  auto CC_start_time = steady_clock::now();
  auto s = vg.acquire_version();
  CC(s.graph);
  auto CC_end_time = steady_clock::now();
  vg.release_version(std::move(s));
  auto CC_time_secs = (duration<double, std::ratio<1, 1>>(
      CC_end_time - CC_start_time)).count();  
  query_file << "aspen_top,100,0," << CC_time_secs << "\n";
  
  auto end_time = steady_clock::now();

  auto runtime_secs = (duration<double, std::ratio<1, 1>>(
    end_time - start_time)).count();
  auto updates_per_second = num_updates / runtime_secs;
  std::cout << "Updates per second: " << updates_per_second << "\n";
  
  exit(EXIT_SUCCESS);
}
