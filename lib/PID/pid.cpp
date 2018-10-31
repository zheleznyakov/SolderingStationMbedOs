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
pid::pid(max6675 &m,int kp_):max(m)
{

    kp = kp_;
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
    int error;

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