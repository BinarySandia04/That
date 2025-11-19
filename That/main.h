#pragma once

#include <vector>

#include <ThatLib/Libs/packages.h>

#include "resource.h"

void InitProject();
void InitPackage();
void WriteFiles(std::vector<Resource>);
void PrintPackageInfo(ThatLib::Package *);
void PrintPackage(ThatLib::Package *);
void PrintBindStatus(std::string letter, ThatLib::Binding* bind);
void ShowBinds(ThatLib::Package *);
