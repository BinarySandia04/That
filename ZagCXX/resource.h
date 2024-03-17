#pragma once

#include <cstddef>
#include <string>

class Resource {
public:
  Resource(const char *start, const char *end, std::string path)
      : mData(start), mSize(end - start), path(path) {}
  const char *const &data() const { return mData; }
  const size_t &size() const { return mSize; }
  const char *begin() const { return mData; }
  const char *end() const { return mData + mSize; }

  void RunScript();
  std::string string();
  std::string path;

private:
  const char *mData;
  size_t mSize;
};
#define LOAD_RESOURCE(x, p)                                                       \
  ([]() {                                                                      \
    extern const char _binary_##x##_start, _binary_##x##_end;                  \
    return Resource(&_binary_##x##_start, &_binary_##x##_end, p);                 \
  })()
