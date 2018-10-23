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

#include "PowerControl.h"

PowerControl::PowerControl(PinName ZeroCross,PinName h1, PinName h2, PinName h3, PinName h4, PinName hup):ZeroCross_(ZeroCross), h1_(h1), h2_(h2), h3_(h3), h4_(h4), h5_(hup)
{
    // 120 - значение, при котором тестовая лапочка очень тускло горит
    dim1 = 120;
    dim2 = 120;
    dim3 = 120;
    dim4 = 120;
    dimup = 120;

    
    ZeroCross_.rise(callback(this,&PowerControl::Crossing)); // инициализация прерывания для детектора 0

}
// Crossing() - обработчик прерывания от детектора 0; взводит пять таймеров для каждого из симисторов
// 75 - коэффицент для перевода значения dim в микросекунды, взят с запасом, при dim=128 получаем 9600мкс
void PowerControl::Crossing()
{
    t1.attach_us(callback(this,&PowerControl::DimHeater1UP),dim1*75);
    t2.attach_us(callback(this,&PowerControl::DimHeater2UP),dim2*75);
    t3.attach_us(callback(this,&PowerControl::DimHeater3UP),dim3*75);
    t4.attach_us(callback(this,&PowerControl::DimHeater4UP),dim4*75);
    t5.attach_us(callback(this,&PowerControl::DimHeater5UP),dimup*75);
    
    
} 
// SetDimming задает задержки включения для каждого из симисторов от 0 до 128
void PowerControl::SetDimming(int d1, int d2, int d3, int d4, int dup)
{
    dim1 = d1;
    dim2 = d2;
    dim3 = d3;
    dim4 = d4;
    dimup = dup;
}
// DimHeater1UP() обработчик таймера, включающий перывый симистор
void PowerControl::DimHeater1UP()
{
            h1_ = 1; // подаем на управлюящий первым симистором выход единицу
            t1.detach();// взводим таймер заново, чтобы через 500мкс снять управляющий сигнал с симистора
            t1.attach_us(callback(this,&PowerControl::DimHeater1Down),500);
}
// DimHeater1Down() обработчик преывания, отключающий управляющий сигнал первого симистора
// сам симистор отключится не сразу, а как только синусоида перейдет через 0 значение
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

