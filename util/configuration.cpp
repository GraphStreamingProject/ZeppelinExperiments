#include <cstdio>
#include <fstream>

#include "configuration.h"

// function which modifies the configuration files
// while not changing paramaters not specified to be changed
void write_configuration(struct sys_config config) {
  // buffering.conf
  std::ofstream conf("./buffering.conf");
  std::ifstream backup_conf("./backup-buffering.conf");
  std::string line;
  if (backup_conf.is_open()) {
    while(getline(backup_conf, line)) {
      if (line[0] == '#' || line[0] == '\n') continue;
      if (line.substr(0, line.find('=')) == "gutter_factor" && config.gutter_factor != 0)
        conf << "gutter_factor=" << config.gutter_factor << std::endl;
      else
        conf << line << std::endl;
    }
  } else {
    printf("ERROR: backup_configuration() must be called before write_configuration.\n");
    exit(EXIT_FAILURE);
  }
  conf.close();
  backup_conf.close();

  // streaming.conf
  conf.open("./streaming.conf");
  backup_conf.open("./backup-streaming.conf");
  if (backup_conf.is_open()) {
    while(getline(backup_conf, line)) {
      if (line[0] == '#' || line[0] == '\n') continue;
      if (line.substr(0, line.find('=')) == "num_groups" && config.num_groups != 0)
        conf << "num_groups=" << config.num_groups << std::endl;
      else if (line.substr(0, line.find('=')) == "group_size" && config.group_size != 0)
        conf << "group_size=" << config.group_size << std::endl;
      else
        conf << line << std::endl;
    }
  } else {
    printf("ERROR: backup_configuration() must be called before write_configuration.\n");
    exit(EXIT_FAILURE);
  }
}

// backup the current configuration to be restored later
void backup_configuration() {
  rename("./buffering.conf", "./backup-buffering.conf");
  rename("./streaming.conf", "./backup-streaming.conf");
}

// restore a previously backed up configuration
void restore_configuration() {
  rename("./backup-buffering.conf", "./buffering.conf");
  rename("./backup-streaming.conf", "./streaming.conf");
}
