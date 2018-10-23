#ifndef MAX6675_h
#define MAX6675_h

#include "mbed.h"

class max6675
{
    SPI& spi;
    DigitalOut ncs;// pin CS для выбора чипа
  public:
  
    max6675(SPI& _spi, PinName _ncs); 
    void select(); //захват линии SPI для данного чипа
    void deselect();// освобождение SPI
    
    float read_temp(); //чтение температуры
  private:
    PinName _CS_pin;
    PinName _SO_pin;
    PinName _SCK_pin;
};

#endif