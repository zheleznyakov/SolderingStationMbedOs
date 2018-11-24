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
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

int SPIflag;
Serial s(PA_2,PA_3);//tx,rx связь с компьютером по uart
Serial s2(PB_6,PA_10);// tx, rx связь с экраном nextion по uart
Thread th1; // поток для обработки комманд от дисплея
// регулятор мощности PowerControl(PinName ZeroCross, PinName h1, PinName h2, PinName h3, PinName h4, PinName hup) 
PowerControl P(D4,D11,D12,D13,D14,D15);
SPI spi(PB_15, PB_14, PB_13); // MOSI, MISO, SCLK интерфейс для термопар
max6675 max_sensor(spi,PB_1); // SPI, CS - chip select первая термопара
max6675 max_sensor2(spi,PA_8); // SPI, CS - chip select вторая термопара
max6675 max_sensor_overheat(spi,PB_10); // SPI, CS - chip select термопара для измерения температуры корпуса

pid reg(max_sensor,P, 7,100,0, &SPIflag); // ПИД регулятор pid(max6675 obj, powercontrol obj, kp, kd, ki)
int displayPage, prevPage; // текущая страница, на которой находится дисплей

/* Дисплей nextion не умеет запоминать точки на графике
 * как только мы ушли со страницы с графиком, точки стираются. graphDown и graphUp хранят точки графика для обеих термопар
 * graphPre хранит значение температуры преднагрева платы для отрисовки на графике (зеленая линия)
 * graphSold хранит значение температуры пайки для гарфика (красная линия)
 * Для того, чтобы добавлять новые точки в массив не сдвигая его, есть переменная graphPos, которая скользит по массиву,
 * указывая на самый старый элемент, который можно переписывать новым значением
*/
int graphDown[309]; // график низ
int graphUp[309]; // график верх
int graphPre; //значение температуры преднагрева
int graphSold; //значение температуры пайки
int graphPos; 

void ShowPage2();
//SDBlockDevice sd(PC_3,PC_2,PC_7,PC_15);
//FATFileSystem fs("fs");



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
    int c = 0; // c>0, если пришли данные, требующие обработки. Если c=0, то пришли ненужные нам данные(служебная информация)

    while(1) {
        while (s2.readable()) {

            if (s2.getc()!='x') // если первый символ не x, то эти данные нам не нужны
            {
                char b;
                while (s2.readable())
                {
                    b=s2.getc();
                    s.printf("data=%c", b);
                }
            }
            else 
            {
                s2.scanf("%s",a); // считываем данные в буффер а
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
                data = atoi(a+pos+1);

                if (command=="sd") //sd - set down. Команда устанавливает температуру для нагрева нижним нагревателем
                {
                    // по команде sd открывается страница с графиками, сейчас нужно будет передавать массивы точек
                     
                    
                    graphPre = data; // зеленая линия на графике, указывающая температуру до которой нужно нагреть
                    ThisThread::sleep_for(200); // пауза нужна для того, чтобы дисплей распознал команду(он только что передавал данные, теперь ему нужно их читать)
                    ShowPage2();
                    reg.SetTemperature(data); // даем задание ПИД регулятору
                }
                if (command == "page") // если была нажата кнопка перехода на другую страницу дисплея
                {
                        prevPage = displayPage;
                        displayPage = data; 
                        if (data == 2){ShowPage2();}
                        else{
                            while(!s2.writable()){ThisThread::sleep_for(5);}
                            s2.printf("page %d%c%c%c",data,255,255,255);// отправляем команду на смену страницы
                        }
                }
                if (command == "back")
                {
                    displayPage = prevPage;
                        if (prevPage==2)
                        {
                            ShowPage2();
                        }
                        else
                        {
                            while(!s2.writable()){ThisThread::sleep_for(5);}
                            s2.printf("page %d%c%c%c",prevPage,255,255,255);// отправляем команду на смену страницы
                        }
                }
                if (command == "toggle")
                {
                    while(!s2.writable()){ThisThread::sleep_for(5);}
                    int x=P.ToggleHeater(data);
                    if (x==0)
                    {
                        if(s2.writable())
                            s2.printf("bt%d.bco=12678%c%c%c",data,255,255,255);
                        else
                            P.ToggleHeater(data);
                    }
                    if (x==1)
                    {
                        if(s2.writable())
                            s2.printf("bt%d.bco=64520%c%c%c",data,255,255,255);
                        else
                            P.ToggleHeater(data);
                    }
                }
                //s.printf("%s, data=%d", a,data); //(для отладки)выводим в com порт компьютера полученную команду и данные
                
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
    graphPre = 15;
    displayPage = 0;
    SPIflag = false; //spi не занят
    ThisThread::sleep_for(200);

    // инициализируем массивы для графиков текущей температурой
    int tdown = max_sensor.read_temp();
    int tup = max_sensor2.read_temp();
    for (int i=0;i<=308;i++)
    {
        graphDown[i] = tdown;
        graphUp[i] = tup;
    }
    graphPos=0;

    /*sd.init();
    fs.mount(&sd);

    FILE* fd = fopen("/fs/hi.txt", "w");
    fprintf(fd, "hello!");
    fclose(fd);

    sd.deinit();
    fs.unmount();*/

    
    // запускаем поток для приема команд от дисплея
    th1.start(ReadCommands);

    int temp, tempu, tempc; // текущая температура. temp, tempu - для контрольных термопар; tempc - для температуры корпуса 

    // начальные значения для первой страницы дисплея (на которой слайдер)
    if (s2.writable())
    {
        s2.printf("h1.val=%d%c%c%c",15,255,255,255);
        s2.printf("man_down.val=%d%c%c%c",15,255,255,255);
    }
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
        // записываем текущую температуру в массивы для отображения на графиках
        graphDown[graphPos] = temp;
        graphUp[graphPos] = tempu;
        graphPos++;
        if (graphPos>308) graphPos=0;


        if (s2.writable())
        {
            if (displayPage == 1){
                // tempn - значение температуры, отображаемое на экране Nextion (экран со слайдером) 
                s2.printf("tempn.val=%d%c%c%c",temp,255,255,255);
            }
            if (displayPage==2)
            {
                // отправляем точку на график
                s2.printf("add 1,0,%d%c%c%c",temp,255,255,255);
                s2.printf("add 1,1,%d%c%c%c",tempu,255,255,255);
                // tempz - значение текущей температуры, отображаемое на странице Nextion с графиком
                s2.printf("tempz.val=%d%c%c%c",temp,255,255,255);
                s2.printf("tempzup.val=%d%c%c%c",tempu,255,255,255);
                s2.printf("tempzcase.val=%d%c%c%c",tempc,255,255,255);

            }
            
            
        }
    }
}
void ShowPage2()
{
    // устанавливаем текущую странцу дисплея в 0, чтобы не шёл лишний обмен данными с экраном
    // на нулевой странице нет обновляемых данных
    prevPage = displayPage;
    displayPage=0;
    while(!s2.writable()){ThisThread::sleep_for(5);} 
    s2.printf("ref_stop%c%c%c",255,255,255); // команда сообщает дисплею, что пока не нужно обновлять инф. на дисплее
    s2.printf("page %d%c%c%c",2,255,255,255);// отображаем 2 страницу на дисплее
    int j=graphPos;// узнаем текущее положение слайдера на массиве
    for (int i=0;i<309;i++) // на график влазит 309 точек
    {
        if (j>308) j=0;
        s2.printf("add 1,0,%d%c%c%c",graphDown[j],255,255,255); //синяя линия
        s2.printf("add 1,1,%d%c%c%c",graphUp[j],255,255,255); //желтая линия
        s2.printf("add 1,3,%d%c%c%c",graphPre,255,255,255); //зеленая линия
        ThisThread::sleep_for(2);
        j++;
    }
    s2.printf("ref_star%c%c%c",255,255,255); // разрешаем дисплею отобразить изменения
    displayPage = 2; 
}
