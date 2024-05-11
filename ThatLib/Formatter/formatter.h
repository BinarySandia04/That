#pragma once

#include <string>
#include <unordered_map>

namespace ThatCXX {
class Formatter {
public:
  std::string Format(std::string, std::unordered_map<int, std::string>);
};
}; // namespace ThatCXX
