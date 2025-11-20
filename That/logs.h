#pragma once

#include <stdint.h>
#include <string>

namespace That {
namespace Logs {

void Print(std::string);
void Error(std::string);
void Success(std::string);
void Warning(std::string);
void Info(std::string);
void Debug(std::string);
void Printf(const char *fmt, ...);

std::string Ask(std::string);
}; // namespace Logs
}; // namespace ThatLib
