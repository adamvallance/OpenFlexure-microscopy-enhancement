#ifndef GTEST_ENDTEST
#define GTEST_ENDTEST


#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>
#include <stdlib.h>
#include <thread>

#include "imageProcessor.h"
#include "frame.h"

class EndTester : public imageProcessor{
    public:
        frame currentFrame;
        void receiveFrame(frame newFrame){     
            //std::cout<<"RECEIVED FRAME";     
            currentFrame.copyFrom(&newFrame);
        }

        //overriding virtual functions which are not required
        void updateSettings(std::map<std::string, std::string> in){return;}
        std::string getParamLabel(){return "";};

};


#endif //GTEST_ENDTEST
