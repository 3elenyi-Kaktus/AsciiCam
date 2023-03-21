#include <opencv2/imgproc.hpp>
#include "webcam_capture.h"
#include "thread"
#include "chrono"

class Cam {
public:

    Cam() {
        cam.open(0, cv::CAP_ANY);
    }
    cv::Mat frame;
    cv::VideoCapture cam;
};

Cam cam;

cv::Mat GetFrame() {
    //cv::Mat frame;
    //--- INITIALIZE VIDEOCAPTURE
    //cv::VideoCapture cam;
    // open the default camera using default API
    // cam.open(cv::CAP_V4L);
    // OR advance usage: select any API backend
    //int deviceID = 0;             // 0 = open default camera
    //int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    //cam.open(deviceID, apiID);
    // check if we succeeded
//    if (!cam.isOpened()) {
//        std::cerr << "ERROR! Unable to open camera\n";
//        exit(-1);
//    }
    //--- GRAB AND WRITE LOOP
    //std::cout << "Start grabbing" << std::endl << "Press any key to terminate" << std::endl;
    // wait for a new frame from camera and store it into 'frame'
    cam.cam.read(cam.frame);
    // check if we succeeded
    if (cam.frame.empty()) {
        std::cerr << "ERROR! blank frame grabbed\n";
        exit(-1);
    }
    //std::cout << "got frame size: " << frame.rows << "x" << frame.cols << std::endl;
//    cv::cvtColor(frame, frame, cv::COLOR_RGB2GRAY);
    //cv::resize(frame, frame,cv::Size(),0.1, 0.1,cv::INTER_AREA);
    // show live and wait for a key with timeout long enough to show images
    //std::cout << "got frame size: " << frame.rows << "x" << frame.cols << std::endl;

    //cv::imshow("Live", frame);

    // the camera will be deinitialized automatically in VideoCapture destructor
    return cam.frame;
}