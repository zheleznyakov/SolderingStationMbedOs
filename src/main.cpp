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
#include <string>


Serial s(PA_2,PA_3);//tx,rx связь с компьютером по uart
Serial s2(PB_6,PA_10);// tx, rx связь с экраном nextion по uart
Thread th1, downHeater;
// регулятор мощности PowerControl(PinName ZeroCross, PinName h1, PinName h2, PinName h3, PinName h4, PinName hup) 
PowerControl P(D4,D11,D12,D13,D14,D15);
SPI spi(PB_15, PB_14, PB_13); // MOSI, MISO, SCLK
max6675 max_sensor(spi,PB_1); // SPI, CS - chip select
max6675 max_sensor2(spi,PA_8); // SPI, CS - chip select
max6675 max_sensor_overheat(spi,PB_10); // SPI, CS - chip select

pid reg(max_sensor,P, 7,50,0.5); 


/*
* ReadCommands ожидает поступление данных от touch панели и обрабатывает команды
* мои команды начинаются с 'x'
* пока есть только одна команда xsd=X, где xsd - команда (set down heater), X - температура(2 байта)  
*/
void ReadCommands()
{
    char a[30]; // буфер для приема команд
    string str,command;
    int data = 0;
    int c = 0;

    while(1) {
        while (s2.readable()) {

            if (s2.getc()!='x') // если первый символ не x, то эти данные нам не нужны
            {
                while (s2.readable())
                    s2.getc();
            }
            else 
            {
                s2.scanf("%s",a); 
                c++;
            }

        }
        if (c>0)
            str.append(a);
        c=0;
        if (str.length()>0)
        {
            int pos =str.find("=");
            if (pos!=-1)
            {
                command = str.substr(0,pos);
                data = a[pos+2];
                data = data<<8;
                data = data|a[pos+1];
                if (command=="sd")
                {
                    reg.SetTemperature(data);
                }
                s.printf("%s, data=%d", command.c_str(),data); //(для отладки)выводим в com порт компьютера полученную команду и данные
                
            }
        }
        str.clear();
        command.clear();
        data = 0;
    }
    
}


int main()
{

    s.baud(115200); // связь с комьютером
    s2.baud(115200);// связь с nextion
    //SetDimming(нагр1, нагр2, нагр3, нагр4, верх)
    //0-мин мощность 249-максимальная при 250 симистор не удерживается открытым
    P.SetDimming(0,0,0,0,0); 

    reg.SetTemperature(15);
    
    // запускаем поток для приема команд от дисплея
    th1.start(ReadCommands);
    // запускаем поток для ПИД регулятора низа
    //downHeater.start(DownHeat);

    int temp, tempu, tempc; // текущая температура

    if (s2.writable())
    {
        s2.printf("h1.val=%d%c%c%c",15,255,255,255);
        s2.printf("man_down.val=%d%c%c%c",15,255,255,255);
    }
    while(1) {
        ThisThread::sleep_for(500);

        temp = reg.temp();
        ThisThread::sleep_for(200);
        tempu = max_sensor2.read_temp();
        ThisThread::sleep_for(200);
        tempc = max_sensor_overheat.read_temp();


        if (s2.writable())
        {
            // tempn - значение температуры, отображаемое на экране Nextion (экран со слайдером) 
            s2.printf("tempn.val=%d%c%c%c",temp,255,255,255);
            // отправляем три одинаковых точки на график для того, чтобы график шёл с более быстрой скоростью
            s2.printf("add 1,0,%d%c%c%c",temp,255,255,255);
                //s2.printf("add 1,0,%d%c%c%c",temp,255,255,255);
                //s2.printf("add 1,0,%d%c%c%c",temp,255,255,255);
            // tempz - значение текущей температуры, отображаемое на странице Nextion с графиком
            s2.printf("tempz.val=%d%c%c%c",temp,255,255,255);
            s2.printf("tempzup.val=%d%c%c%c",tempu,255,255,255);
            s2.printf("tempzcase.val=%d%c%c%c",tempc,255,255,255);
        }
    }
}