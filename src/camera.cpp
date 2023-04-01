#include "camera.h"

void Camera::threadLoop(){
    while(isOn){
        postFrame();
    }
}

void Camera::postFrame(){
    if(frameCb==nullptr) return;
    cv::Mat capture;
    videoCapture.read(capture);
    if (capture.empty()) {
        std::cerr << "OpenFlexure Error! blank frame grabbed\nPlease check your PiCamera microscope module is connected.<n";
        frameCb==nullptr;
        this-> stop(); // stop processing. To avoid endless loop
        throw 1;
        //exit(1);
    } 
    
    //Create frame instance
    frame f;
    f.doMeta = doMeta;

    //turn back off for next frame
    if (doMeta){
        doMeta = false;
    }

    f.image = capture;
    //outputs f to the receiveFrame method of the next object in the dataflow through a callback
    frameCb -> receiveFrame(f);

}

void Camera::start(int deviceID, int apiID){
    isOn=true;
    videoCapture.open(deviceID, apiID); 
    cameraThread = std::thread(&Camera::threadLoop, this);
}

void Camera::stop(){
    isOn=false;
    cameraThread.join();
}

void Camera::registerCallback(imageProcessor* cb){
    frameCb = cb;
}

bool Camera::getIsOn(){
    return isOn;    //method to check status of camera
}

void Camera::captureMetadata(){
    doMeta = true;
}



