#include <array>
#include <cassert>
#include <cstdio>

#include "shell_exec.h"

std::string shell_exec(const std::string cmd) {
  std::array<char, 128> buffer;
  std::string result;
  auto pipe = popen(cmd.c_str(), "r");

  if (!pipe)
    throw std::runtime_error("popen() failed!");

  while (!feof(pipe)) {
    if (fgets(buffer.data(), 128, pipe) != nullptr)
      result += buffer.data();
  }

  auto rc = pclose(pipe);

  std::string error_str = std::string("Shell command failed: ") + cmd;
  assert(((void)error_str, rc == EXIT_SUCCESS));

  return result;
}
