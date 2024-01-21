#pragma once

#include <string>

namespace Zag {

class Error {
public:
  Error(int, int, std::string, std::string);
  void Print(std::string);
  void PrintInline();
  void PrintSourceLine(std::string, int);
  void PrintErrorSourceLine(std::string, int, int, int);

private:
  int position;
  int length;
  std::string content;
  std::string fileName;

  // Setted when printing the error
  void SetLineNum(std::string);
  std::string source;
  int line;
  int column;
};

} // namespace Zag
