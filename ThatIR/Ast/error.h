#pragma once

#include <string>

namespace ThatIR {

class Error {
public:
  Error();
  Error(int, int, std::string, std::string);
  void Print(std::string);
  void PrintInline();
  void PrintSourceLine(std::string, int);
  void PrintErrorSourceLine(std::string, int, int, int);

private:
  bool fired;

  int position;
  int length;
  std::string content;
  std::string fileName;

  // Setted when printing the error
  void SetLineNum();
  void CountLines();

  std::string source;
  int line;
  int column;
  int lineCount;
};

} // namespace Zag
