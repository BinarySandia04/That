#include <filesystem>
#ifdef __linux__
#include <pwd.h>
#include <sys/types.h>
#endif


#ifndef THAT_PATH
#define THAT_PATH std::filesystem::path(getenv("HOME")) / ".zag/"
#endif

#ifndef THAT_VERSION
#define THAT_VERSION "0.1"
#endif
