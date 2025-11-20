#include "constant.h"

#include <That/logs.h>

using namespace Thatc;
using namespace That;

void Constant::print(){
  switch(type){
    case T_INT: Logs::Printf("%d", value.i); break;
    case T_NUMBER: Logs::Printf("%f", value.d); break;
    case T_STRING: Logs::Printf("%s", *value.s); break;
    case T_BOL: {
                  if(value.b) Logs::Printf("Yep");
                  else Logs::Printf("Nop");
                } break;
    case T_NIL: Logs::Printf("Nil"); break;
    default: Logs::Printf("???");
  }
}
