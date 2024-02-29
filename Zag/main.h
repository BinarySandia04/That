#pragma once

#include <vector>

#include <ZagIR/Libs/packages.h>

#include "src/resource.h"

void InitProject();
void InitPackage();
void WriteFiles(std::vector<Resource>);
void PrintPackageInfo(ZagIR::Package *);
void PrintPackage(ZagIR::Package *);
void PrintBindStatus(std::string letter, ZagIR::Binding* bind);
void ShowBinds(ZagIR::Package *);
