#pragma once

#include "cstdint"

#define STRINGTOW(x) std::wstring(x.begin(), x.end())

#define WTOSTRING(x) std::string(x.begin(), x.end())

struct Coordinates {
    int64_t y;
    int64_t x;
};

struct Size {
    int64_t height;
    int64_t width;
};