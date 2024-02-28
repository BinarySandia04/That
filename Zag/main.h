#pragma once

#include <vector>

#include <ZagIR/Libs/packages.h>

#include "src/resource.h"

void InitProject();
void InitLib();
void WriteFiles(std::vector<Resource>);
void PrintPackageInfo(ZagIR::Package *);
