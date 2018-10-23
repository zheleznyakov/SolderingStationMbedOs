/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* Класс PowerControl представляет программную реализацию пятиканального фазового регулятора
* который представляет собой симисторный регулятор мощности из пяти симисторов. В этом регуляторе
* присутствует детектор 0, который выдает сигнал 5 вольт, как только синусоида пересекла 0.
* В этом момент срабатывает прерывание ZeroCross_, которое взводит пять таймеров на время от 0 до 10000мкс
* 10000мкс - время полупериода колебаний тока в розетке 50Гц. Чем позже сработает таймер, тем меньшую мощность 
* получим в нагрузке.
*/

#ifndef PowerControl_H
#define PowerControl_H

#include "mbed.h"

class PowerControl
{
private:
    /* data */
    int dim1,dim2,dim3, dim4, dimup; //переменные для задержки от 0 до 128 (0-0мкс, 128 - 10000мкс)

    Ticker t1,t2,t3,t4,t5; // таймеры для влкючения и выключения симисторов
    InterruptIn ZeroCross_;// прерывание срабатывает по сигналу от детектора 0

    DigitalOut h1_,h2_,h3_,h4_,h5_; // выводы для включения симисторов
    void Crossing(void); // обработчик прерывания детектора 0
    void DimHeater1UP(); // Обработчик таймера, включающий симистор
    void DimHeater1Down();// обработчик таймера, выключающий симистор
    void DimHeater2UP();
    void DimHeater2Down();
    void DimHeater3UP();
    void DimHeater3Down();
    void DimHeater4UP();
    void DimHeater4Down();
    void DimHeater5UP();
    void DimHeater5Down();
      
public:
/*
* Конструктор PowerControl 
* ZeroCross - пин, на который приходит сигнал с детектора 0
* h1...hup - пины, на которых формируется управляющий сигнал для симисторов
*/
    PowerControl(PinName ZeroCross, PinName h1, PinName h2, PinName h3, PinName h4, PinName hup);

    void SetDimming(int d1, int d2, int d3, int d4, int dup); // Задает задержки для каждого из каналов от 0 до 128 (0-0мкс, 128 - 10000мкс)
};




#endif