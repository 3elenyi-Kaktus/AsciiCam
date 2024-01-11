#pragma once

#include <ostream>
#include "cstdint"

#define STRINGTOW(x) std::wstring(x.begin(), x.end())

#define WTOSTRING(x) std::string(x.begin(), x.end())

struct Coordinates {
    Coordinates() : y(0), x(0) {}

    Coordinates(int64_t y_, int64_t x_) : y(y_), x(x_) {}

    friend std::ostream& operator<<(std::ostream& stream, Coordinates obj) {
        stream << "(" << obj.y << ", " << obj.x << ")";
        return stream;
    }

    bool operator==(const Coordinates& other) const {
        return y == other.y && x == other.x;
    }

    int64_t y;
    int64_t x;
};

struct Size {
    Size() : height(0), width(0) {}

    Size(int64_t height_, int64_t width_) : height(height_), width(width_) {}

    friend std::ostream& operator<<(std::ostream& stream, Size obj) {
        stream << obj.height << "x" << obj.width;
        return stream;
    }

    bool operator==(const Size& other) const {
        return height == other.height && width == other.width;
    }

    int64_t height;
    int64_t width;
};