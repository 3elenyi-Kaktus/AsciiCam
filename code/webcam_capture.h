#pragma once
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include "opencv4/opencv2/imgproc.hpp"
#include <iostream>
#include "logger.h"

const int ASCII_SYMBOL_HEIGHT = 15;
const int ASCII_SYMBOL_WIDTH = 9;

class Cam {
public:
    explicit Cam(Logger &logger);

    cv::Mat GetFrame();

    void GetNewFrame(Logger &logger);

    void PreProcessFrame(int height, int width);

private:
    cv::Mat frame;
    cv::VideoCapture cam;
};