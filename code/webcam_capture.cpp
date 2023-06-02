#include "webcam_capture.h"

std::pair<int, int> ConvertSizeToSymb(std::pair<int, int> &size) {
    return {size.first / ASCII_SYMBOL_HEIGHT, size.second / ASCII_SYMBOL_WIDTH};
}

std::pair<int, int> ConvertSizeToPx(std::pair<int, int> &size) {
    return {size.first * ASCII_SYMBOL_HEIGHT, size.second * ASCII_SYMBOL_WIDTH};
}

WebCamera::WebCamera(Logger &logger) {
    is_initialized = false;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // auto api
    cam.open(deviceID, apiID);
    // check if we succeeded
    if (!cam.isOpened()) {
        logger << "ERROR! Unable to open camera\n";
        return;
    }
    logger << "Camera initialized successfully\n";
    is_initialized = true;
}

cv::Mat &WebCamera::GetFrame() {
    return frame;
}

void WebCamera::GetNewFrame(Logger &logger) {
    cam.read(frame);
    // check if we succeeded
    if (frame.empty()) {
        logger << "ERROR! blank frame grabbed\n";
    }
}

std::pair<int, int> WebCamera::GetFittedFrameSize(int height, int width) {
    // convert size of the area we want to fit in from symbols to pixels
    height *= ASCII_SYMBOL_HEIGHT;
    width *= ASCII_SYMBOL_WIDTH;
    // calculate the height/width ratio of the frame
    double hw_ratio = frame.rows / static_cast<double>(frame.cols);

    // first possible scaling
    int width_1 = width;
    int height_1 = static_cast<int>(std::lround(width_1 * hw_ratio));

    // second possible scaling
    int height_2 = height;
    int width_2 = static_cast<int>(std::lround(height_2 / hw_ratio));

    int new_height, new_width;

    // choose the scaling which fits in the area
    if (height_1 > height) {
        new_height = height_2;
        new_width = width_2;
    } else if (width_2 > width) {
        new_height = height_1;
        new_width = width_1;
    }

    return {new_height, new_width};
}

void WebCamera::PreprocessFrame(std::pair<int, int> &size) {
    cv::Size frame_size(size.second, size.first);
    // first resize
    cv::resize(frame, frame, frame_size, cv::INTER_AREA);

    // another scaling of the image for the correct mapping between pixels and symbols
    std::pair<int, int> size_symb = ConvertSizeToSymb(size);
    cv::Size frame_size_symb(size_symb.second, size_symb.first);
    cv::resize(frame, frame, frame_size_symb, cv::INTER_AREA);

    // convert image to grayscale
    cv::cvtColor(frame, frame, cv::COLOR_RGB2GRAY);
}