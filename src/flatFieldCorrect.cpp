// This file performs flat field and chromatic abberation correction on the input image, to compensate for uneven illumination on the image plane

// Author Mark Main

#include "flatFieldCorrect.h"


#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>



/**
* Recieves new frames for processing via callback.
**/
void flatFieldCorrect::receiveFrame(frame newFrame) {
    if (!enabled){ 

        newFrame.setParameter(paramLabel, "OFF");
        frameCb->receiveFrame(newFrame);
        return;
    }
    // do stuff here

    // passing frame into the flat field functions
    flatField(newFrame); 

}

/**
* Implemented from ImageProcessor. Updates settings based on metadata.
* @param metadata standard map of strings containing metadata
**/
void flatFieldCorrect::updateSettings(std::map<std::string, std::string> metadata){
    
    std::string rec = metadata[paramLabel];

    bool desired = (rec == "ON");


    if (enabled != desired){
        toggleEnable();
    }

    
}

void flatFieldCorrect::updateAverage(frame f) {
    averageCalculated = false;
    // Load reference images
    std::string pathname = getenv("HOME");
    pathname += + "/cellUviewGallery/.FlatFieldGallery/";
    std::vector<cv::Mat> reference_images;
    for (int i = 0; i <= 19; i++) {     // iterating through 20 captures taken for flat field correction 
        std::string flatFieldPath = pathname + "flatField" + std::to_string(i) + ".jpg";
        cv::Mat reference_image = cv::imread(flatFieldPath);
        reference_images.push_back(reference_image);
    }

    // Calculate the average of the reference images
    cv::Mat average_reference_image;
    // initialise average reference to be 1/20 weighted average of first two captures
    // weighted average calculation: output_array = alpha*array1 + beta*array2 + gamma
    // where alpha and beta are weighting coefficients of each array, gamma = 0
    double weight_coeff = 1.0/20;   // coefficient for weighted average calculations
    cv::addWeighted(reference_images[0], weight_coeff, reference_images[1], weight_coeff, 0, average_reference_image);
    for (int i = 2; i < 19; i++) { //add further captures at 1/20 weighting to form weighted average.
        cv::addWeighted(average_reference_image, 1.0, reference_images[i], weight_coeff, 0, average_reference_image);
    }   


    // Calculate the correction factor using the average reference image and apply some blurring
    cv::Mat correction_factor;
    cv::GaussianBlur(average_reference_image, average_reference_image, cv::Size(35, 35), 0);
    cv::resize(average_reference_image, correction_factor, f.image.size());

    //Create a white matrix to generate an inversion and subtract the correction factor from this value
    cv::Mat whiteImage(f.image.rows, f.image.cols, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::subtract(whiteImage, correction_factor, correction_factor);

    current_correction_factor = correction_factor;
    averageCalculated = true;

}

/**
* Sets flag to recalibrate the flat field average based on 20 captures.
* Called through user interface.
**/
void flatFieldCorrect::setUpdateFlag(){
    calculateAverageEnabled = true;
    averageCalculated=false;
}

void flatFieldCorrect::flatField(frame f) {
    cv::Mat corrected_image;
    

    if (calculateAverageEnabled == true) {
        calculateAverageEnabled = false;
        averageCalculated=false;
        updateAverage(f);
    }
    //don't apply if average calculation is not complete
    if (averageCalculated == false){
        frameCb->receiveFrame(f);
        return;
    }

    //generate Matrices identical to input and correction at 16 bit to scale beyond 255 to normalise resultatn values
    cv::Mat f_16uc3, current_correction_factor_16uc3;
    f.image.convertTo(f_16uc3, CV_16UC3);
    current_correction_factor.convertTo(current_correction_factor_16uc3, CV_16UC3);
    cv::add(f_16uc3, current_correction_factor_16uc3, current_correction_factor_16uc3);

    // Find the maximum value in current_correction_factor_16uc3
    double maxVal;
    cv::minMaxLoc(current_correction_factor_16uc3, nullptr, &maxVal);

    // Compute the scaling factor
    double scaleFactor = 255.0 / maxVal;

    // Scale current_correction_factor_16uc3 by the scaling factor
    current_correction_factor_16uc3 *= scaleFactor;
    current_correction_factor_16uc3.convertTo(f.image, CV_8UC3);

    f.setParameter(paramLabel, "ON");
    frameCb->receiveFrame(f);
}
