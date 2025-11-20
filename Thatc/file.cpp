#include "file.h"

#include <string>
#include <cstdio>

int readFile(const char* path, std::string* result) {
    FILE* f = fopen(path, "rb");
    if (!f) return 1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    std::string data(size, '\0');     // allocate string
    fread(&data[0], 1, size, f);      // read into string buffer

    fclose(f);
    *result = data;
    return 0;
}
