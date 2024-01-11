#include "video_gallery.h"
#include "../ascii_converter.h"

VideoGallery::VideoGallery(const Coordinates &win_position_, const Size &win_size_, ScreenManager &manager_, WebCamera& camera_) :
        Window(win_position_, win_size_),
        manager(&manager_),
        camera(camera_) {
    std::pair<int, int> converter = {size.height, size.width};
    converter = ConvertSizeToPx(converter);
    fitted_video_size = {converter.first, converter.second};
}

void VideoGallery::updateWindow(cv::Mat &frame) {
    std::vector<std::vector<u_char>> ascii_matrix;
    if (!frame.empty()) {
        std::pair<int, int> sz = {fitted_video_size.height, fitted_video_size.width};
        ascii_matrix = ConvertFrameToASCII(camera.PreprocessFrame(frame, sz));
    } else {
        std::string msg = "here could be frame...";
        ascii_matrix[0] = std::vector<u_char>(msg.begin(), msg.end());
        sleep(1);
    }
    Coordinates p = position;
    manager->mvChunkPrint(p, ascii_matrix);
    manager->refreshScreen(true);
}

void VideoGallery::Resize(Size new_size) {
    std::pair<int, int> converter = {new_size.height, new_size.width};
    converter = ConvertSizeToPx(converter);
    fitted_video_size = {converter.first, converter.second};
}
