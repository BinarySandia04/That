#include "op_codes.h"

#include <string>

using namespace Thatc;

const char* Thatc::toString(OpCode c){
  switch(c) {
    case OP_RET: return "RET";
    case OP_LOADK: return "LOADK";
    default: return "?????";
  }
}
