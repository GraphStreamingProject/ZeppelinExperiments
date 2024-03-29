#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cmath>

#include "../graph/api.h"
#include "../algorithms/CC.h"

#include "BufferedEdgeInput.h"

int main (int argc, char * argv[])
{
  if (argc != 6)
  {
    std::cout << "Incorrect number of arguments!" << std::endl;
    std::cout << "Arguments are: stream_file, batch_size, file_buffer_size, timeout_hrs, output_file" << std::endl;
    exit(EXIT_FAILURE);
  }
  unsigned long buff_size;
  sscanf(argv[3], "%lu", &buff_size);

  BufferedEdgeInput buff_in{argv[1], buff_size};

  std::ofstream out_file(argv[5]);
  if (!out_file) {
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
        std::cout << "TIMEOUT: exiting early after: " << i << " insertions" << std::endl;
        num_updates = i;
        break;
      }
      status_idx = 0;
    }
    if (log_count >= hundredth)
    {
      // Record ingestion progress

      auto new_log_time = steady_clock::now();

      auto log_interval_secs = (duration<double, std::ratio<1, 1>>(new_log_time - prev_log_time)).count();
      auto updates_per_second = log_count / log_interval_secs;
      time_so_far_secs += log_interval_secs;
      log_count = 0;

      prev_log_time = steady_clock::now();
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
  
  auto ingest_end_time = steady_clock::now();

  auto CC_start_time = steady_clock::now();
  auto s = vg.acquire_version();
  // std::cout << s.graph.num_vertices() << " " << s.graph.num_edges() << std::endl;
  CC(s.graph);
  auto CC_end_time = steady_clock::now();

  auto ingest_time_secs = (duration<double, std::ratio<1, 1>>(
    ingest_end_time - start_time)).count();
  auto updates_per_second = num_updates / ingest_time_secs;
  
  auto CC_time_secs = (duration<double, std::ratio<1, 1>>(
      CC_end_time - CC_start_time)).count();

  out_file << updates_per_second << " " << CC_time_secs << " " << num_nodes << std::endl;

  exit(EXIT_SUCCESS);
}
