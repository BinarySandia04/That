#pragma once

#include <vector>

#include <ThatIR/Libs/packages.h>

#include "src/resource.h"

void InitProject();
void InitPackage();
void WriteFiles(std::vector<Resource>);
void PrintPackageInfo(ThatIR::Package *);
void PrintPackage(ThatIR::Package *);
void PrintBindStatus(std::string letter, ThatIR::Binding* bind);
void ShowBinds(ThatIR::Package *);
