#include "pid.h"

pid::pid(max6675 &m,int kp_):max(m)
{
    //max = m;
    kp = kp_;
    requered_temp=30;
    power = 0;
    
    //Thread::wait(250000);
    //t_cumpute.start(callback(this,&pid::Compute));
    //tim.attach_us(callback(this,&pid::Compute),250000);
    //t_cumpute = new Thread(pid::Compute);
    //t_cumpute = new Thread(Compute,this);
    //t_cumpute->set_priority(osprioryt)
    tim2= new RtosTimer(Compute, this);
    tim2->start(250);
}
float pid::ReadTemp()
{
    return current_temp;
}
void pid::SetTemperature(float t_)
{
    requered_temp = t_;
}
void pid::Compute(void const *arguments)
{
    int error;
    //Thread::wait(1000);

    pid *self = (pid*)arguments;
    //self->current_temp +=0.1;
    
    //current_temp = max.read_temp();
    self->current_temp = self->temp();
        
    error = self->requered_temp-self->current_temp;
    if (error>0){
        self->power = self->kp*error;
        if (self->power>249) self->power = 249;
    }
    else{
        self->power = 0;
    }
    
    
}
int pid::Power()
{
    return power;
}
float pid::temp()
{
    return max.read_temp();
}