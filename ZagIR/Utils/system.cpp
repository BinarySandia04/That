#include "system.h"

#include <string>
#include <stdexcept>
#include <stdio.h>

std::string ZagIR::Utils::Exec(std::string command) {
  char buffer[128];
  std::string result = "";

  // Open pipe to file
  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return "popen failed!";
  }

  // read till end of process:
  while (!feof(pipe)) {

    // use buffer to read and add to result
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }

  pclose(pipe);
  return result;
}
