#pragma once

// structure containing the configuration options
// we may wish to specify for experiments
// zero means don't change config param
struct sys_config {
  int num_groups    = 0;
  int group_size    = 0;
  int gutter_factor = 0;
};

// function which modifies the configuration files
void write_configuration(struct sys_config _conf);

// backup the current configuration to be restored later
void backup_configuration();

// restore a previously backed up configuration
void restore_configuration();
