/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* 
*/
#include "mbed.h"
#include "rtos.h"

#include "PowerControl.h"
#include "max6675.h"


char readData[20];
char buf[20];
DigitalOut led(LED1);
Serial s(PA_2,PA_3);//tx,rx communication with pc
Serial s2(PB_6,PA_10);
Thread th1;



void ReadCommands()
{
    int counter = 0;
    while(1) {
        while (s2.readable()) {
            buf[counter]= s2.getc();
            counter++;
        }
        if (counter>0){
            s.printf("%s",buf);
            counter = 0;
        }
    }
    
}


int main()
{
    PowerControl P(D4,D11,D12,D13,D14,D15);
    P.SetDimming(110,110,110,110,110); //0-макс. яркость 124 -минимальная

    SPI spi(PB_15, PB_14, PB_13); // MOSI, MISO, SCLK
    max6675 max(spi,PB_1);
    

    s.baud(115200);
    s2.baud(115200);
    
    th1.start(ReadCommands);
    int i=0;
    int temp;
    while(1) {
        Thread::wait(1000);
        //wait(1);
        temp = max.read_temp();
        
        s2.printf("tempn.val=%d%c%c%c",temp,255,255,255);
        i++;
    }
}