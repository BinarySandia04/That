#include "scope.h"

using namespace That;

Scope::Scope() {
  data = new ObjectContainer();
}

Scope::Scope(Scope *before) {
  data = new ObjectContainer(before->data);
}

Scope::~Scope() {
  if(data != nullptr) delete data;
}

void Scope::Print() {
  data->Print(0);
}

Object* Scope::GetObject(std::string key){
  return data->Fetch(key);
}

void Scope::AddObject(std::string key, Object* obj){
  data->AddObject(key, obj);
}

void Scope::AddObject(std::string key, Object* obj, Privacy privacy){
  data->AddObject(key, obj, privacy);
}

bool Scope::Exists(std::string key){
  return data->Exists(key);
}

void Scope::Detach(){
  data = nullptr;
}
