

#include "PowerControl.h"

PowerControl::PowerControl(PinName ZeroCross,PinName h1, PinName h2, PinName h3, PinName h4, PinName hup):ZeroCross_(ZeroCross), h1_(h1), h2_(h2), h3_(h3), h4_(h4), h5_(hup)
{
    z1 = 0; z2 = 0; z3 = 0; z4 = 0; z5 = 0;
    dim1 = 120;
    dim2 = 120;
    dim3 = 120;
    dim4 = 120;
    dimup = 120;

    
    ZeroCross_.rise(callback(this,&PowerControl::Crossing));
    //heater1.start(callback(this,&PowerControl::DimHeater1));
     // heater2.start(callback(this,&PowerControl::DimHeater2));
}

void PowerControl::Crossing()
{
    t1.attach_us(callback(this,&PowerControl::DimHeater1UP),dim1*75);
    t2.attach_us(callback(this,&PowerControl::DimHeater2UP),dim2*75);
    t3.attach_us(callback(this,&PowerControl::DimHeater3UP),dim3*75);
    t4.attach_us(callback(this,&PowerControl::DimHeater4UP),dim4*75);
    t5.attach_us(callback(this,&PowerControl::DimHeater5UP),dimup*75);
    
    
} 

void PowerControl::SetDimming(int d1, int d2, int d3, int d4, int dup)
{
    dim1 = d1;
    dim2 = d2;
    dim3 = d3;
    dim4 = d4;
    dimup = dup;
}

void PowerControl::DimHeater1UP()
{
            h1_ = 1;
            t1.detach();
            t1.attach_us(callback(this,&PowerControl::DimHeater1Down),500);
}
void PowerControl::DimHeater1Down()
{
    h1_ = 0;
    t1.detach();
}
void PowerControl::DimHeater2UP()
{
            h2_ = 1;
            t2.detach();
            t2.attach_us(callback(this,&PowerControl::DimHeater2Down),500);
}
void PowerControl::DimHeater2Down()
{
    h2_ = 0;
    t2.detach();
}
void PowerControl::DimHeater3UP()
{
            h3_ = 1;
            t3.detach();
            t3.attach_us(callback(this,&PowerControl::DimHeater3Down),500);
}
void PowerControl::DimHeater3Down()
{
    h3_ = 0;
    t3.detach();
}
void PowerControl::DimHeater4UP()
{
            h4_ = 1;
            t4.detach();
            t4.attach_us(callback(this,&PowerControl::DimHeater4Down),500);
}
void PowerControl::DimHeater4Down()
{
    h4_ = 0;
    t4.detach();
}
void PowerControl::DimHeater5UP()
{
            h5_ = 1;
            t5.detach();
            t5.attach_us(callback(this,&PowerControl::DimHeater5Down),500);
}
void PowerControl::DimHeater5Down()
{
    h5_ = 0;
    t5.detach();
}

void PowerControl::DimHeater2()
{
    while(true)
    {
        if (z1)
        {
            
            t1.attach_us(callback(this,&PowerControl::DimHeater1UP),dim1*75);
            t2.attach_us(callback(this,&PowerControl::DimHeater2UP),dim2*75);
            //t3.attach_us(callback(this,&PowerControl::DimHeater3UP),dim3*75);
            //t4.attach_us(callback(this,&PowerControl::DimHeater4UP),dim4*75);
            //t5.attach_us(callback(this,&PowerControl::DimHeater5UP),dimup*75);
            z1=0;
        }
    }

}