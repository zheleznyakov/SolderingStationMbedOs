#include "display.h"
Display::Display(Serial &s):com(s)
{
    displayPage = 0;
    prevPage = 0;
    graphPre=15;
}

void Display::ShowPage2()
{
    // устанавливаем текущую странцу дисплея в 0, чтобы не шёл лишний обмен данными с экраном
    // на нулевой странице нет обновляемых данных
    prevPage = displayPage;
    displayPage=0;
    while(!com.writable()){ThisThread::sleep_for(5);} 
    com.printf("ref_stop%c%c%c",255,255,255); // команда сообщает дисплею, что пока не нужно обновлять инф. на дисплее
    com.printf("page %d%c%c%c",2,255,255,255);// отображаем 2 страницу на дисплее
    int j=graphPos;// узнаем текущее положение слайдера на массиве
    for (int i=0;i<309;i++) // на график влазит 309 точек
    {
        if (j>308) j=0;
        com.printf("add 1,0,%d%c%c%c",graphDown[j],255,255,255); //синяя линия
        com.printf("add 1,1,%d%c%c%c",graphUp[j],255,255,255); //желтая линия
        com.printf("add 1,3,%d%c%c%c",graphPre,255,255,255); //зеленая линия
        ThisThread::sleep_for(2);
        j++;
    }
    com.printf("ref_star%c%c%c",255,255,255); // разрешаем дисплею отобразить изменения
    displayPage = 2; 
}
void Display::ShowPage(int n)
{
    prevPage = displayPage; // запоминаем текущую страницу
    displayPage = n; // запоминаем страницу на которую будем переходить
    if (n==2)
    {
        ShowPage2();
    }
    else{
        while(!com.writable()){ThisThread::sleep_for(5);}
        com.printf("page %d%c%c%c",n,255,255,255);// отправляем команду на смену страницы

    }
}
void Display::Back()
{
    displayPage = prevPage;
    ShowPage(displayPage);
}
void Display::ToggleHeater(int heater,bool value)
{
    while(!com.writable()){ThisThread::sleep_for(5);}
    if (value)
    {
        com.printf("bt%d.bco=64520%c%c%c",heater,255,255,255); // меняем цвет на оранжевый (включен)
    }
    else
    {
        com.printf("bt%d.bco=12678%c%c%c",heater,255,255,255); // меняем цвет на серый (выключен)
    }

}
void Display::Init(int temp_down,int temp_up)
{
    for (int i=0;i<=308;i++)
    {
        graphDown[i] = temp_down;
        graphUp[i] = temp_up;
    }
    graphPos=0;
    // начальные значения для первой страницы дисплея (на которой слайдер)
    while(!com.writable()){ThisThread::sleep_for(5);}
    com.printf("h1.val=%d%c%c%c",15,255,255,255);
    com.printf("man_down.val=%d%c%c%c",15,255,255,255);
}
void Display::ShowInf(int temp_down,int temp_up,int temp_case)
{
    // записываем текущую температуру в массивы для отображения на графиках
    graphDown[graphPos] = temp_down;
    graphUp[graphPos] = temp_up;
    graphPos++;
    if (graphPos>308) graphPos=0;

    if (displayPage == 1){
        while(!com.writable()){ThisThread::sleep_for(5);}
                // tempn - значение температуры, отображаемое на экране Nextion (экран со слайдером) 
        com.printf("tempn.val=%d%c%c%c",temp_down,255,255,255);
    }
    if (displayPage==2)
    {
        while(!com.writable()){ThisThread::sleep_for(5);}
        // отправляем точку на график
        com.printf("add 1,0,%d%c%c%c",temp_down,255,255,255);
        com.printf("add 1,1,%d%c%c%c",temp_up,255,255,255);
        // tempz - значение текущей температуры, отображаемое на странице Nextion с графиком
        com.printf("tempz.val=%d%c%c%c",temp_down,255,255,255);
        com.printf("tempzup.val=%d%c%c%c",temp_up,255,255,255);
        com.printf("tempzcase.val=%d%c%c%c",temp_case,255,255,255);
    }
}
void Display::SetPreheatTemp(int t)
{
    graphPre = t;
}