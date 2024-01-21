#pragma once

#include <string>

namespace Zag {

class Error {
public:
  Error(int, std::string);
  void Print();

private:
  int position;
  std::string content;
};

} // namespace Zag
