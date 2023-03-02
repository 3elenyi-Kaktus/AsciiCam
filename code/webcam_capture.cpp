#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <iostream>

int main() {
    cv::Mat frame;
    //--- INITIALIZE VIDEOCAPTURE
    cv::VideoCapture cam;
    // open the default camera using default API
    // cam.open(cv::CAP_V4L);
    // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cam.open(deviceID, apiID);
    // check if we succeeded
    if (!cam.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    //--- GRAB AND WRITE LOOP
    std::cout << "Start grabbing" << std::endl
         << "Press any key to terminate" << std::endl;
    for (;;) {
        // wait for a new frame from camera and store it into 'frame'
        cam.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        // show live and wait for a key with timeout long enough to show images
        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}