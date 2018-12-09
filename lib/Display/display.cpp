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
    if (n!=9)
    {
        if (!(displayPage==9&&n==6))
        prevPage = displayPage; // запоминаем текущую страницу
    }
    displayPage = n; // запоминаем страницу на которую будем переходить
    if (n==2)
    {
        ShowPage2();
        return;
    }
    
    while(!com.writable()){ThisThread::sleep_for(5);}
    com.printf("page %d%c%c%c",n,255,255,255);// отправляем команду на смену страницы
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
int Display::GetCurrentPageNumber()
{
    return displayPage;
}
void Display::ShowProfilesListPage(Profile_Id_Title *all_profiles)
{
    int x=0;
    while(all_profiles)
    {
        while(!com.writable()){ThisThread::sleep_for(5);}
        if (x<=3)
            com.printf("profilesList.profile%d.txt=\"%s\"%c%c%c",all_profiles->id,all_profiles->title.c_str(),255,255,255);
        else
            com.printf("profilesList2.profile%d.txt=\"%s\"%c%c%c",all_profiles->id,all_profiles->title.c_str(),255,255,255);
        x++;
        all_profiles = all_profiles->next;

    }
}
void Display::ShowSelectedProfile(ProfilePoint *points, string ProfileName)
{
    int x=-1;
    while(points)
    {
        if (points->type=="up")
        {
            x++;
            while(!com.writable()){ThisThread::sleep_for(5);}
            com.printf("point%d.txt=\"UP%ddeg\"%c%c%c",x,points->value,255,255,255);
        }
        if (points->type=="down")
        {
            x++;
            while(!com.writable()){ThisThread::sleep_for(5);}
            com.printf("point%d.txt=\"Down%ddeg\"%c%c%c",x,points->value,255,255,255);
        }
        if (points->type=="wait")
        {
            x++;
            while(!com.writable()){ThisThread::sleep_for(5);}
            com.printf("point%d.txt=\"Wait%ds\"%c%c%c",x,points->value,255,255,255);
        }
        if (points->type=="none")
        {
            x++;
            while(!com.writable()){ThisThread::sleep_for(5);}
            com.printf("point%d.txt=\"none\"%c%c%c",x,255,255,255);
        }
        points = points->next;

    }
    while(!com.writable()){ThisThread::sleep_for(5);}
    com.printf("profilename.txt=\"%s\"%c%c%c",ProfileName.c_str(),255,255,255);
    
}
void Display::ClearPointsPage()
{
    for (int i=0;i<=5;i++)
    {
        com.printf("prof_props.point%d.txt=\"none\"%c%c%c",i,255,255,255);
    }
}
void Display::ShowPointPage(string type,int val)
{
    ShowPage(9);
    if (type=="up")
    {
        while(!com.writable()){ThisThread::sleep_for(5);}
        com.printf("pointsSets.check0.val=1%c%c%c",255,255,255);
        com.printf("pointsSets.check1.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check2.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check3.val=0%c%c%c",255,255,255);
    }
    if (type=="down")
    {
        while(!com.writable()){ThisThread::sleep_for(5);}
        com.printf("pointsSets.check0.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check1.val=1%c%c%c",255,255,255);
        com.printf("pointsSets.check2.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check3.val=0%c%c%c",255,255,255);
    }
    if (type=="wait")
    {
        while(!com.writable()){ThisThread::sleep_for(5);}
        com.printf("pointsSets.check0.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check1.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check2.val=1%c%c%c",255,255,255);
        com.printf("pointsSets.check3.val=0%c%c%c",255,255,255);
    }
    if (type=="none")
    {
        while(!com.writable()){ThisThread::sleep_for(5);}
        com.printf("pointsSets.check0.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check1.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check2.val=0%c%c%c",255,255,255);
        com.printf("pointsSets.check3.val=1%c%c%c",255,255,255);
    }
    com.printf("pointsSets.point_value.val=%d%c%c%c",val,255,255,255);
    
}
