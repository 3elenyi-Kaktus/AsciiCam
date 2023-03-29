#pragma once
#include <opencv4/opencv2/core.hpp>
#include "vector"
#include "unordered_map"
#include "climits"
#include "cmath"

typedef unsigned char u_char;

std::vector<std::vector<u_char>> ConvertFrameToASCII(cv::Mat &frame);