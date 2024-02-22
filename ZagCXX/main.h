#pragma once

#include <filesystem>
#include <string>

void TranspileFile(std::string);
void Transpile(std::string, std::string);
int Compile(std::filesystem::path, std::filesystem::path);
void Run(std::filesystem::path);
