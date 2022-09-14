#include <cstdio>
#include <fstream>

#include "configuration.h"

// function which modifies the configuration files
// while not changing paramaters not specified to be changed
GraphConfiguration create_graph_config(struct sys_config _conf) {
  auto config = GraphConfiguration();
  if (_conf.num_groups != 0) {
    config.num_groups(_conf.num_groups);
  }
  if (_conf.group_size != 0) {
    config.group_size(_conf.group_size);
  }
  if (_conf.gutter_factor != 0) {
    config.gutter_conf().gutter_factor(_conf.gutter_factor);
  }
  config.gutter_sys(_conf.use_tree ? GUTTERTREE : STANDALONE);
  config.backup_in_mem(_conf.backup_mem);
  return config;
}
