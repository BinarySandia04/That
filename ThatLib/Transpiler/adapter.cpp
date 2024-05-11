#include "adapter.h"

#include <iostream>

using namespace ThatLib;

Adapter::Adapter(){

}

Adapter::~Adapter(){

}

void Adapter::OpenBlock() {}
void Adapter::CloseBlock() {}
void Adapter::Int(std::string) {}
void Adapter::Num(std::string) {}
void Adapter::Str(std::string) {}
void Adapter::Yep() {}
void Adapter::Nop() {}
void Adapter::Chr(std::string) {}
void Adapter::Id(std::string) {}
void Adapter::Binary(std::string) {}
