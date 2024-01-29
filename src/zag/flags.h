#pragma once

#include <cstdint>

enum Flags {
  NONE = 1 << 0,
  DEBUG = 1 << 1,
};

constexpr enum Flags operator |( const enum Flags selfValue, const enum Flags inValue )
{
    return (enum Flags)(uint32_t(selfValue) | uint32_t(inValue));
}

extern Flags programFlags;
