#pragma once

#include <string>
#include <vector>

namespace That {
class Data {
  public:
    std::string type;
    void* value;
    std::vector<Data*> children;
};
}; // namespace ThatLib
