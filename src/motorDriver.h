#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <iostream>
#include <stdlib.h>
#include <thread>

#include <wiringSerial.h>
#include <unistd.h>


class MotorCallback {
public:
    virtual void returnPosition(int x, int y, int z) = 0;
};


class MotorDriver {

public:
    MotorDriver() = default;     // constructor

    void registerCallback(MotorCallback* cb);

    //void start(std::string device = "/dev/ttyUSB0", int baud = 115200);
    void start(const char* device = "/dev/ttyUSB0", int baud = 115200);
    void stop();

    void resetToZero();
    void getPosition();
    void mov(char axis, int inc);


private:

    void run();

    MotorCallback* motorCb = nullptr;
    bool enabled = false;

    int fd = 0;
    std::string currentPosition = "0 0 0";
    int positionArray[3]; 
    std::string commandAck = "";
    int bytesToRead;
    

    std::thread motorThread;

    char firmwareVer[28];
    char dataRead[];


};



#endif //MOTOR_DRIVER_H
