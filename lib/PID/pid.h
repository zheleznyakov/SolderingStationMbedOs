/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* Класс pid представляет реализацию ПИД регулятора для нагревателя
* Класс содержит таймер, который постоянно вычисляет требуемую мощность нагревателя
* исходя из текущей и требуемой температуры.
*/
#ifndef PID_H
#define PID_H

#include "mbed.h"
#include "max6675.h"
#include "PowerControl.h"

class pid
{
private:
    double kp; // коэффицент пропорционального регулятора
    double ki; 
    int kd;
    int previousError;
    double integral;
    int *spiFlag;
    max6675 &max; // ссылка на объект термопары
    PowerControl &pcontrol; // ссылка на фазовый регулятор
    float requered_temp; // требуемая температура
    volatile float current_temp;// текущая температура
    int maxPower=200;

    RtosTimer *tim2; // таймер вызывает функцию Compute для вычисления мощности
    volatile int power; // рассчитанная мощность

    static void Compute(void const *arguments); // функция вычисляет можность исодя из заданной и текущей температуры
public:
    pid(max6675 &m, PowerControl &pc,double kp_, int kd_, double ki_, int *flag);
    float ReadTemp(); // возвращает текущую температуру не опрашивая термопару
    void SetTemperature(float t_); // задает требуемую температуру
    int Power(); // возвращает рассчитанную мощность
    float temp(); // возвращает текущую температуру из датчика max6675
    void setMaxPower(int x);
};

#endif