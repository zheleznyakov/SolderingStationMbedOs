#include <mbed.h>
#include "max6675.h"

max6675::max6675(SPI& _spi, PinName _ncs) : spi(_spi), ncs(_ncs) {

}

    float max6675::read_temp() {
    short value = 0;
    float temp = 0;
    
    uint8_t highByte=0;
    uint8_t lowByte=0;
    
    select();  //выбор чипа устанавливает CS в 0
    //Thread::wait(250);
    //wait(.25); //Эта задержка нужна для обновления данных о температуре

    highByte = spi.write(0); // отправляем два пустых
    lowByte = spi.write(0);  // байта, при этом одновременно получаем два байта с данными 
    deselect(); //устанавливаем CS в 1 (освобождаем SPI интерфейс) после того как считали температуру.


    if (lowByte & (1<<2)) {  // проверяем третий бит, если равен 1, то термопара отключена
        return -1;
    } else {
        value = (highByte << 5 | lowByte>>3); // выделим из полученных данных инф. о температуре (с 3 по 14 бит)
    }

    temp = (value*0.25); // умножаем значение на чтобы получить ˚C или
                         //  * (9.0/5.0)) + 32.0;   // чтобы получить ˚F 

return temp;
}

void max6675::select() {
    ncs = 0;
}

void max6675::deselect() {
    ncs = 1;
}