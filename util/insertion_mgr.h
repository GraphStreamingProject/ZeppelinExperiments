#pragma once

/**
 * @return the ingestion rate, insertions per second 
 */
double perform_insertions(std::string binary_input, std::string output_file, sys_config config, long timeout=0);
