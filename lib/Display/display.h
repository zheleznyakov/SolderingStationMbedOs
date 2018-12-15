#ifndef DISPLAY_H
#define DISPLAY_H

#include "mbed.h"
#include "profile.h"
class Display
{
    Serial &com;
    int displayPage;
    int prevPage;
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

    public:
        Display(Serial &s);
        void ShowPage2();//страница с графиком
        void ShowPage(int n);
        void Back();
        void ToggleHeater(int heater,bool value);
        void Init(int temp_down,int temp_up);
        void ShowInf(int temp_down,int temp_up,int temp_case, int sec);
        void SetPreheatTemp(int t);
        int GetCurrentPageNumber();
        void ShowProfilesListPage(Profile_Id_Title *all_profiles);
        void ShowSelectedProfile(ProfilePoint *points, string ProfileName);
        void ClearPointsPage();
        void ShowPointPage(string type,int val);
        void ShowCurrentPoint(string profileName,string type, int val);


};

#endif