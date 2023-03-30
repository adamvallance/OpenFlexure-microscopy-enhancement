#ifndef OPENFLEXURE_CAMERA_H
#define OPENFLEXURE_CAMERA_H


#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <stdlib.h>
#include <thread>
#include "imageProcessor.h"

#include "frame.h"


class Camera{
public:
    Camera() = default;
    void start(int deviceID = 0, int apiID=0);
    void stop();
    //void receiveFrame(frame newFrame);
    bool getIsOn();
    void registerCallback(imageProcessor*);
    
private:
    frame f;
    void postFrame();
    void threadLoop();
    cv::VideoCapture videoCapture;
    std::thread cameraThread;
    bool isOn = false;
    imageProcessor* frameCb = nullptr;

};

#endif // OPENFLEXURE_CAMERA_H