#pragma once

#include <string>
#include <vector>

namespace ZagIR {
class Data {
  public:
    std::string type;
    void* value;
    std::vector<Data*> children;
};
}; // namespace ZagIR
