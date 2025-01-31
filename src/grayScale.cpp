#include "grayScale.h"
#include <opencv2/imgproc.hpp>

/**
* Recieves new frames for processing via callback.
**/
void grayScale::receiveFrame(frame newFrame) {
    if (!enabled){
        newFrame.setParameter(paramLabel, "OFF");
        frameCb->receiveFrame(newFrame);
        return;
    }

    // Pass frame into the erosion function
    grayEnhance(newFrame); 
}

void grayScale::grayEnhance(frame f) {
    // Convert input frame to cv::Mat
    cv::Mat input_mat(f.image.rows, f.image.cols, CV_8UC3, f.image.data);

    // Create output cv::Mat
    cv::Mat output_mat(input_mat.size(), CV_8UC3);

    // Convert input frame to grayscale
    cv::Mat gray_frame;
    cv::cvtColor(input_mat, gray_frame, cv::COLOR_BGR2GRAY);

    // Copy grayscale frame to output
    cv::cvtColor(gray_frame, output_mat, cv::COLOR_GRAY2BGR);

    //Add output to frame
    f.image = output_mat;

    // Output the frame through the callback onto the next instance in the dataflow
    f.setParameter(paramLabel, "ON");
    frameCb->receiveFrame(f);
}

/**
* Implemented from ImageProcessor. Updates settings based on metadata.
* @param metadata standard map of strings containing metadata
**/
void grayScale::updateSettings(std::map<std::string, std::string> metadata){
    
    std::string rec = metadata[paramLabel];

    bool desired = (rec == "ON");
    // std::cout<<rec<<std::endl;

    // std::cout<<desired<<std::endl;

    if (enabled != desired){
        toggleEnable();
    }

    
}