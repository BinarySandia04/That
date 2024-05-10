#pragma once

#include <string>
#include <vector>

namespace ThatIR {
class Data {
  public:
    std::string type;
    void* value;
    std::vector<Data*> children;
};
}; // namespace ThatIR
