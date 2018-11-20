/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* Класс pid представляет реализацию ПИД регулятора для нагревателя
* Класс содержит таймер, который постоянно вычисляет требуемую мощность нагревателя
* исходя из текущей и требуемой температуры.
*/

#include "pid.h"

/* pid::pid(max6675 &m,int kp_) 
*  в конструктор передаем ссылку m на темопару max6675, ссылку pc на фазовый регулятор и коэффиценты регулятора kp_, kd_, ki_
*  flag - флаг SPI 1- занят; 0 - свободен
*/
pid::pid(max6675 &m, PowerControl &pc,int kp_, int kd_, double ki_, int *flag):max(m), pcontrol(pc)
{

    kp = kp_;
    kd = kd_;
    ki = ki_;
    previousError = 0;
    integral =0;
    requered_temp=30; //заданная температура по умолчанию
    power = 0; 
    spiFlag = flag;
    
    // tim2- таймер, который считывает температуру и вычисляет мощность по алгоритму ПИД регулятора
    tim2= new RtosTimer(Compute, this);
    tim2->start(5000);
}
float pid::ReadTemp()
{
    return current_temp;
}
void pid::SetTemperature(float t_)
{
    requered_temp = t_;
    integral = 0;
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
    self->integral+=self->ki*(double)error;
    x+=self->integral;
    if (x>0){
        if (x<=200) self->power = x;;
        if (x>200) self->power = 200;
    }
    else{
        self->power = 0;
    }
    self->pcontrol.SetDimming(self->power,self->power,self->power,self->power,1);
}

int pid::Power()
{
    return power;
}
float pid::temp()
{
    while (*spiFlag){wait_ms(10);}
    *spiFlag = 1;
    current_temp = max.read_temp();
    *spiFlag = 0;
    return current_temp;
    
}
