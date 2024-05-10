#pragma once

#include <stdint.h>
#include <string>

namespace ThatIR {
namespace Logs {
class Color {
  public:
  Color(uint8_t r, uint8_t g, uint8_t b);
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

void Print(std::string);
void Error(std::string);
void Success(std::string);
void Warning(std::string);
void Info(std::string);
void Debug(std::string);

std::string Ask(std::string);
std::string color(Color c);
void Gradient(std::string, Color, Color);
}; // namespace Logs
}; // namespace ThatIR
