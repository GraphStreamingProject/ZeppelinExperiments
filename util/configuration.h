#pragma once

#include <graph_configuration.h>

// structure containing the configuration options
// we may wish to specify for experiments
// for int values, zero means don't change config param
struct sys_config {
  int num_groups    = 0;
  int group_size    = 0;
  int gutter_factor = 0;
  bool use_tree     = 0;
  bool backup_mem   = 0;
};

// function which modifies the configuration files
GraphConfiguration create_graph_config(struct sys_config _conf);
