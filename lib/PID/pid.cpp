/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* Класс pid представляет реализацию ПИД регулятора для нагревателя
* Класс содержит таймер, который постоянно вычисляет требуемую мощность нагревателя
* исходя из текущей и требуемой температуры.
*/

#include "pid.h"

/* pid::pid(max6675 &m,int kp_) 
*  в конструктор передаем ссылку m на темопару max6675 и коэффицент регулятора kp_
*/
pid::pid(max6675 &m,int kp_, int kd_, int ki_):max(m)
{

    kp = kp_;
    kd = kd_;
    ki = ki_;
    previousError = 0;
    integral =0;
    requered_temp=30; //заданная температура по умолчанию
    power = 0; 
    
    // tim2- таймер, который считывает температуру и вычисляет мощность по алгоритму ПИД регулятора
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
    pid *self = (pid*)arguments;
    int error,x;

    self->current_temp = self->temp();
      
    error = self->requered_temp-self->current_temp;
    x = (error - self->previousError)*self->kd;  
    self->previousError = error;
    x+=error*self->kp;
    self->integral+=self->ki*error;
    x+=self->integral;
    if (x>0){
        if (x<=249) self->power = x;;
        if (x>249) self->power = 249;
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
