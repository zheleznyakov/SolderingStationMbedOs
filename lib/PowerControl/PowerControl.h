/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* 
*/

#ifndef PowerControl_H
#define PowerControl_H

#include "mbed.h"

class PowerControl
{
private:
    /* data */
    int dim1,dim2,dim3, dim4, dimup;

    Thread heater1, heater2, heater3, heater4, heater5; 
    Ticker t1,t2,t3,t4,t5;
    volatile bool z1, z2, z3, z4, z5; //флаги о том, что синусоида прошла через 0 для каждого нагревателя.
    InterruptIn ZeroCross_;

    DigitalOut h1_,h2_,h3_,h4_,h5_;
    void Crossing(void);
    void DimHeater1UP();
    void DimHeater1Down();
    void DimHeater2UP();
    void DimHeater2Down();
    void DimHeater3UP();
    void DimHeater3Down();
    void DimHeater4UP();
    void DimHeater4Down();
    void DimHeater5UP();
    void DimHeater5Down();
    
    void DimHeater2();

   
public:
    PowerControl(PinName ZeroCross, PinName h1, PinName h2, PinName h3, PinName h4, PinName hup);
    ~PowerControl();

    void SetDimming(int d1, int d2, int d3, int d4, int dup);
};




#endif