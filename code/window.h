#pragma once

#include "definitions.h"


class Window {
public:
    Window(Coordinates coords, Size size_) : position(coords), size(size_) {}

    virtual void Resize(Size new_size) = 0;

protected:
    Coordinates position;
    Size size;
};