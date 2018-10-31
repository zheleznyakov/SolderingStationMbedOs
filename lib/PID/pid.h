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
#include "rtos.h"

class pid
{
private:
    int kp; // коэффицент пропорционального регулятора
    max6675 &max; // ссылка на объект термопары
    float requered_temp; // требуемая температура
    volatile float current_temp;// текущая температура

    RtosTimer *tim2; // таймер вызывает функцию Compute для вычисления мощности
    volatile int power; // рассчитанная мощность

    static void Compute(void const *arguments); // функция вычисляет можность исодя из заданной и текущей температуры
public:
    pid(max6675 &m,int kp_);
    float ReadTemp(); // возвращает текущую температуру не опрашивая термопару
    void SetTemperature(float t_); // задает требуемую температуру
    int Power(); // возвращает рассчитанную мощность
    float temp(); // возвращает текущую температуру из датчика max6675

};

#endif