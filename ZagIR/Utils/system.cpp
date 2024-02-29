#include "system.h"

#include <string>
#include <stdexcept>
#include <stdio.h>

std::string ZagIR::Utils::Exec(std::string command) {
  char buffer[128];
  std::string result = "";

  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return "popen failed!";
  }

  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }

  pclose(pipe);
  return result;
}
