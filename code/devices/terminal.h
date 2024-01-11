#pragma once

#include "../definitions.h"
#include "video_driver.h"
#include "../logger.h"
#include "../window.h"
#include "memory"

class Terminal {
public:
    Terminal();

    ScreenManager screen;
    std::vector<std::shared_ptr<Window>> active_windows;
};

void SIGWINCH_handler(int a);