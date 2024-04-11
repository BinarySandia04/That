#include <filesystem>
#ifdef __linux__
#include <pwd.h>
#include <sys/types.h>
#endif


#ifndef ZAG_PATH
#define ZAG_PATH std::filesystem::path(getenv("HOME")) / ".zag/"
#endif

#ifndef ZAG_VERSION
#define ZAG_VERSION "0.1"
#endif
