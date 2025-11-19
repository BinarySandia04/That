#pragma once

#include <stdint.h>
#include <string>

namespace ThatLib {
namespace Logs {

void Print(std::string);
void Error(std::string);
void Success(std::string);
void Warning(std::string);
void Info(std::string);
void Debug(std::string);

std::string Ask(std::string);
}; // namespace Logs
}; // namespace ThatLib
