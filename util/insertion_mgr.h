#pragma once

#include <utility>
#include <graph_zeppelin_common.h>

typedef struct _InsertionData {
  double ingestion_rate; // insertions per sec
  double cc_time; // in seconds
  node_id_t num_nodes; // in the graph
} InsertionData;

InsertionData perform_insertions(std::string binary_input, std::string output_file, sys_config config, long timeout=0);
