#include "webcam_capture.h"

WebCamera::WebCamera(Logger &logger) {
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // auto api
    cam.open(deviceID, apiID);
    // check if we succeeded
    if (!cam.isOpened()) {
        logger << "ERROR! Unable to open camera\n";
    }
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

void WebCamera::PreprocessFrame(int height, int width) {
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
    cv::Size new_size(new_width, new_height);
    // first resize
    cv::resize(frame, frame, new_size, cv::INTER_AREA);

    // another scaling of the image for the correct mapping between pixels and symbols
    int new_height_in_symb = new_height / ASCII_SYMBOL_HEIGHT;
    int new_width_in_symb = new_width / ASCII_SYMBOL_WIDTH;
    cv::Size new_size_symb(new_width_in_symb, new_height_in_symb);
    cv::resize(frame, frame, new_size_symb, cv::INTER_AREA);

    // convert image to grayscale
    cv::cvtColor(frame, frame, cv::COLOR_RGB2GRAY);
}