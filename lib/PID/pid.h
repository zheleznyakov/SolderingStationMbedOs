#ifndef PID_H
#define PID_H

#include "mbed.h"
#include "max6675.h"
#include "rtos.h"

class pid
{
private:
    int kp;
    max6675 &max;
    float requered_temp;
    volatile float current_temp;
    Ticker tim;
    Thread *t_cumpute;
    RtosTimer *tim2;
    volatile int power;

    static void Compute(void const *arguments);
public:
    pid(max6675 &m,int kp_);
    float ReadTemp();
    void SetTemperature(float t_);
    int Power();
    float temp();

};

#endif