#pragma once

#include "ascii_converter.h"
#include "logger.h"
#include "printer.h"
#include "webcam_capture.h"
#include "chrono"
#include "network.h"

void Execute(Logger& logger);

void SelfVideo(Terminal &terminal, WebCamera &camera, Logger &logger);
