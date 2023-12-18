#include <array>
#include <cassert>
#include <cstdio>
#include <stdexcept>

#include "shell_exec.h"

std::string shell_exec(const std::string cmd, bool capture_stdout) {
  std::string result;
  int rc;

  if (!capture_stdout) {
    rc = system(cmd.c_str());
  } else {
    std::array<char, 128> buffer;
    auto pipe = popen(cmd.c_str(), "r");

    if (!pipe)
      throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), 128, pipe) != nullptr)
      result += buffer.data();

    rc = pclose(pipe);

    std::string error_str = std::string("Shell command failed: ") + cmd;
    assert(((void)error_str, rc == EXIT_SUCCESS));

    return result;
  }

  std::string error_str = std::string("Shell command failed: ") + cmd;
  assert(((void)error_str, rc == EXIT_SUCCESS));

  return result;
}
