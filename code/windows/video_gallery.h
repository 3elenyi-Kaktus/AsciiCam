#pragma once

#include "../window.h"
#include "../devices/video_driver.h"
#include "../devices/webcam_capture.h"

class VideoGallery : public Window {
public:
    VideoGallery(const Coordinates &win_position_, const Size &win_size_, ScreenManager &manager_, WebCamera& camera_);

    void updateWindow(cv::Mat &frame);

    void Resize(Size new_size) override;

private:
    ScreenManager *manager;
    Size fitted_video_size;
    WebCamera camera;
};

