#pragma once

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>
#include "logger.h"
#include <cmath>

enum {
    ASCII_SYMBOL_HEIGHT = 15,
    ASCII_SYMBOL_WIDTH = 9
};

class WebCamera {
public:
    explicit WebCamera(Logger &logger);

    cv::Mat &GetFrame();

    void GetNewFrame(Logger &logger);

    void PreprocessFrame(int height, int width);

    bool is_initialized;
private:
    cv::Mat frame;
    cv::VideoCapture cam;
};