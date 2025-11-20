#pragma once

#include <vector>

#include <That/Libs/packages.h>

#include "resource.h"

using namespace That;

void InitProject();
void InitPackage();
void WriteFiles(std::vector<Resource>);
void PrintPackageInfo(Package *);
void PrintPackage(Package *);
void PrintBindStatus(std::string letter, Binding* bind);
void ShowBinds(Package *);
