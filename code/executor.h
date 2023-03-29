#pragma once

#include "ascii_converter.h"
#include "logger.h"
#include "printer.h"
#include "webcam_capture.h"
#include "chrono"

void Execute();

void CamVideo(Terminal &terminal, WebCamera &camera, Logger &logger);
