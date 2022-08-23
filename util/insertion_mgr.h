#pragma once

#include <utility>
#include <graph_zeppelin_common.h>

/**
 * @return <the ingestion rate, insertions per second; CC time, in seconds>; num nodes in graph
 */
std::pair<std::pair<double, double>, node_id_t> perform_insertions(std::string binary_input, std::string output_file, sys_config config, long timeout=0);
