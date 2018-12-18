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
    double kp; // коэффициент пропорционального регулятора
    double ki;  //интегрирующий кожэффициент
    int kd; // дифференцирующий коэффициент
    int previousError; //ошибка на предыдущем шаге для вычисления дифференцирующ. составляющей
    double integral; // накопленная ошибка
    int *spiFlag; // ссылка на флаг, который сигнализирует о том, что spi занят
    max6675 &max; // ссылка на объект термопары
    PowerControl &pcontrol; // ссылка на фазовый регулятор
    float requered_temp; // требуемая температура
    volatile float current_temp;// текущая температура
    int maxPower=200;

    RtosTimer *tim2; // таймер вызывает функцию Compute для вычисления мощности
    volatile int power; // рассчитанная мощность
    bool heaters[5]={0,0,0,0,0};// массив нагревателей. допустим 0,1,0,1,0 - использовать первый и третий нагреватель

    static void Compute(void const *arguments); // функция вычисляет можность исодя из заданной и текущей температуры
public:
    pid(max6675 &m, PowerControl &pc,double kp_, int kd_, double ki_, int *flag);
    float ReadTemp(); // возвращает текущую температуру не опрашивая термопару
    void SetTemperature(float t_); // задает требуемую температуру
    int Power(); // возвращает рассчитанную мощность
    float temp(); // возвращает текущую температуру из датчика max6675
    void setMaxPower(int x); // ограничивает максимальную мощность
    void selectHeaters(bool h1,bool h2,bool h3,bool h4,bool h5);//выбор нагревателей заполняет массив heaters[5]
};

#endif