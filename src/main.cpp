/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* 
*/
#include "mbed.h"
#include "rtos.h"

#include "PowerControl.h"


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
    

    s.baud(115200);
    s2.baud(115200);
    
    th1.start(ReadCommands);
    int i=0;
    while(1) {
        Thread::wait(1000);
        //wait(1);
        
        s2.printf("tempn.val=%d%c%c%c",i,255,255,255);
        i++;
    }
}