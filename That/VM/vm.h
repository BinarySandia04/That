#pragma once

#include <string>

#include <ThatLib/Environment/environment.h>
#include <ThatLib/Adapter/adapter.h>

namespace That {

  class VM : public ThatLib::Adapter {
    
    public:
      VM();
      ~VM();
      void Run(std::string code);

    private:
      Environment* env;
  };

};
