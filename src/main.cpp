/*
* Автор - Железняков Андрей
* Сайт - itworkclub.ru
* 
*/
#include "mbed.h"
#include "rtos.h"

#include "PowerControl.h"
#include "max6675.h"
#include "pid.h"


char readData[20];
char buf[20];
DigitalOut led(LED1);
Serial s(PA_2,PA_3);//tx,rx communication with pc
Serial s2(PB_6,PA_10);
Thread th1, downHeater;

PowerControl P(D4,D11,D12,D13,D14,D15);
SPI spi(PB_15, PB_14, PB_13); // MOSI, MISO, SCLK
max6675 max_sensor(spi,PB_1);
pid reg(max_sensor, 5);

void DownHeat()
{
    int p;
    while(1)
    {
        p = reg.Power();
        P.SetDimming(p, 1,1,1,1);
        Thread::wait(500);
    }

}

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
    //PowerControl P(D4,D11,D12,D13,D14,D15);

    //SetDimming(нагр1, нагр2, нагр3, нагр4, верх)
    //0-мин мощность 249-максимальная при 250 симистор не удерживается открытым
    P.SetDimming(10,1,1,1,1); 


    //SPI spi(PB_15, PB_14, PB_13); // MOSI, MISO, SCLK
    //max6675 max(spi,PB_1);
    //pid reg(&max, 10);
    reg.SetTemperature(70);
    

    s.baud(115200);
    s2.baud(115200);
    
    th1.start(ReadCommands);
    downHeater.start(DownHeat);
    int i=0;
    int temp;
    while(1) {
        Thread::wait(1000);
        //wait(1);
        temp = reg.ReadTemp();
        
        s2.printf("tempn.val=%d%c%c%c",temp,255,255,255);
        i++;
    }
}