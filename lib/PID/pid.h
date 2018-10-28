#ifndef PID_H
#define PID_H

#include "mbed.h"
class pid
{
private:
    int kp;
public:
    pid(int kp_);
    float Compute();

};

#endif