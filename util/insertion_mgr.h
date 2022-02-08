#pragma once
#include <binary_graph_stream.h>

void perform_insertions(std::string binary_input, std::string output_file, sys_config config);

void perform_continuous_insertions(BinaryGraphStream& stream, sys_config config);
