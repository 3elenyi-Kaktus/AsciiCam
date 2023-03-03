#pragma once
#include <opencv4/opencv2/core.hpp>
#include "vector"
#include "unordered_map"
#include "climits"
#include "cmath"

typedef unsigned char u_char;

enum {
    ASCII_SYMBOL_HEIGHT = 18,
    ASCII_SYMBOL_WIDTH = 9
};

u_char GetPixelsBrightness(cv::Mat &frame, size_t height_pos, size_t width_pos);

std::vector<std::vector<u_char>> ConvertFrameToASCII(cv::Mat &frame);