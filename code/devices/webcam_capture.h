#pragma once

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>
#include "../logger.h"
#include <cmath>

enum {
    ASCII_SYMBOL_HEIGHT = 15,
    ASCII_SYMBOL_WIDTH = 9
};

class WebCamera {
public:
    WebCamera();

    cv::Mat GetNewFrame();

    std::pair<int, int> GetFittedFrameSize(cv::Mat &frame, int height, int width);

    cv::Mat& PreprocessFrame(cv::Mat& fr, std::pair<int, int> &size);

    bool is_initialized;

    cv::VideoCapture cam;
};

std::pair<int, int> ConvertSizeToSymb(std::pair<int, int> &size); // params: {h, w}

std::pair<int, int> ConvertSizeToPx(std::pair<int, int> &size); // params: {h, w}