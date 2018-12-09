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
#include "display.h"
#include <string>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "tinyxml.h"
#include "profile.h"

int SPIflag;
Serial s(PA_2,PA_3);//tx,rx связь с компьютером по uart
Serial s2(PC_10,PC_11);// tx, rx связь с экраном nextion по uart
Display disp(s2);
Thread th1; // поток для обработки комманд от дисплея
// регулятор мощности PowerControl(PinName ZeroCross, PinName h1, PinName h2, PinName h3, PinName h4, PinName hup) 
PowerControl P(PC_7,PB_6,PA_7,PA_6,PA_5,PB_9); //zero cross, h1,h2,h3,h4,hup
SPI spi(PB_5, PB_4, PB_3); // MOSI, MISO, SCLK интерфейс для термопар
max6675 max_sensor(spi,PB_10); // SPI, CS - chip select первая термопара
max6675 max_sensor2(spi,PA_8); // SPI, CS - chip select вторая термопара
max6675 max_sensor_overheat(spi,PA_9); // SPI, CS - chip select термопара для измерения температуры корпуса

pid reg(max_sensor,P, 7,100,0, &SPIflag); // ПИД регулятор pid(max6675 obj, powercontrol obj, kp, kd, ki)

SDBlockDevice sd(PB_15,PB_14,PB_13,PC_4);//mosi,miso,sclk,cs
FATFileSystem fs("fs");
Profiles pr;



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
    string datastr;
    int c = 0; // c>0, если пришли данные, требующие обработки. Если c=0, то пришли ненужные нам данные(служебная информация)

    while(1) {
        while (s2.readable()) {

            if (s2.getc()!='x') // если первый символ не x, то эти данные нам не нужны
            {
                while (s2.readable())
                {
                    s2.getc();
                }
            }
            else 
            {
                
                //s2.gets(a,2);
                s2.gets(a,29);
                //s2.scanf("%s",a); // считываем данные в буффер а
                
                s.printf("mas[a]=%s\n\r",a);
                c++;
            }

        }
        if (c>0) // если пришли данные, которые нужно обработать
            str.append(a);
        c=0;
        if (str.length()>0)
        {
            // разбиваем строку на команду и данные. Все что перед равно - команда. После равно - данные
            int pos =str.find("=");
            if (pos!=-1)
            {
                command = str.substr(0,pos); 
                if (command=="name")
                {
                    char *t;
                    t=a+pos+1;
                    datastr.append(t);
                    sd.init();
                    fs.mount(&sd);

                    pr.SetCurrentProfileName(datastr);
                    sd.deinit();
                    fs.unmount();
                    s.printf("ChangeName=%s\n\r",datastr.c_str());
                    datastr="";
                }
                else{
                    data = atoi(a+pos+1);
                }

                if (command=="sd") //sd - set down. Команда устанавливает температуру для нагрева нижним нагревателем
                {
                    // по команде sd открывается страница с графиками, сейчас нужно будет передавать массивы точек
                     
                    
                    disp.SetPreheatTemp(data); // зеленая линия на графике, указывающая температуру до которой нужно нагреть
                    ThisThread::sleep_for(200); // пауза нужна для того, чтобы дисплей распознал команду(он только что передавал данные, теперь ему нужно их читать)
                    disp.ShowPage2(); //функция переключает дисплей на страницу с графиками и передает массивы точек для графиков
                    reg.SetTemperature(data); // даем задание ПИД регулятору
                }
                if (command == "page") // если была нажата кнопка перехода на другую страницу дисплея
                {
                        disp.ShowPage(data);
                }
                if (command == "back") // команда перехода на предыдущую страницу
                {
                   disp.Back();
                }
                if (command == "toggle")  // команда включения/выключения элементов нижнего нагревателя
                {
                    
                    int x=P.ToggleHeater(data);
                    disp.ToggleHeater(data, x);
                }
                if (command == "setprofile")
                {
                    s.printf("Set profile %d\n\r",data);
                    disp.ClearPointsPage();
                    disp.ShowPage(6);
                    pr.SelectProfile(data);
                    disp.ShowSelectedProfile(pr.GetPoints(),pr.GetProfileName());
                    
                    
                }
               
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
    //reg.setMaxPower(10);
    // уставка температуры при включении питания
    reg.SetTemperature(0);

    SPIflag = false; //spi не занят
    ThisThread::sleep_for(200);

    // инициализируем массивы для графиков текущей температурой
    int tdown = max_sensor.read_temp();
    int tup = max_sensor2.read_temp();
    disp.Init(tdown,tup);

    sd.init();
    fs.mount(&sd);

    
    if (pr.init())
    {s.printf("Profiles file loaded\n\r");}
    if (pr.LoadProfiles())
    {s.printf("Profiles loaded\n\r");}
    pr.SelectProfile(0);
    s.printf("ProfileName = %s\n\r",pr.GetProfileName().c_str());
    s.printf("Count of points = %d\n\r",pr.GetCountOfPoints());
    ProfilePoint* points = pr.GetPoints();
    while(points)
    {
        s.printf("Point type = %s; value=%d\n\r",points->type.c_str(),points->value);
        points = points->next;

    }
    

    //FILE* fd = fopen("/fs/hi.txt", "w");
    //fprintf(fd, "hello!");
    //fclose(fd);

    sd.deinit();
    fs.unmount();

    
    // запускаем поток для приема команд от дисплея
    th1.start(ReadCommands);

    int temp, tempu, tempc; // текущая температура. temp, tempu - для контрольных термопар; tempc - для температуры корпуса 

    while(1) {
        ThisThread::sleep_for(1000);

        while (SPIflag){ThisThread::sleep_for(10);}
        SPIflag = 1;
        temp = max_sensor.read_temp();
        ThisThread::sleep_for(10);
        tempu = max_sensor2.read_temp();
        ThisThread::sleep_for(10);
        tempc = max_sensor_overheat.read_temp();
        SPIflag = 0;
        // отображаем необходимую информацию на экране
        if (disp.GetCurrentPageNumber()==5)
        {
            disp.ShowProfilesListPage(pr.GetProfiles());
        }
        disp.ShowInf(temp,tempu,tempc);
    }
}

