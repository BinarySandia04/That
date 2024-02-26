#pragma once

#include <string>
#include <vector>

namespace ZagCXX {
class VarType {
public:
  VarType();
  VarType(std::string name, std::string translation, std::string includes);

  std::string name;
  std::vector<VarType *> children;
  std::string translation;
  std::string includes;

  // Should format the type
  std::string Transpile();

private:
  std::string TranspileChildren();
};
} // namespace ZagCXX
