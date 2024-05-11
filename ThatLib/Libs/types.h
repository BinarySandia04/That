#pragma once

#include <string>
#include <vector>

namespace ThatLib {
class Data {
  public:
    std::string type;
    void* value;
    std::vector<Data*> children;
};
}; // namespace ThatLib
