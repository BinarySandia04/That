#pragma once

#include <cstdint>

namespace Thatc {
  enum OpCode : uint32_t {
    // A, B, C
    

    // A, BX

    OP_RET,

    OP_LOADK,
  };

  const char* toString(OpCode c);
};
