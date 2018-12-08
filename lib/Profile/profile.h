/*
* класс Profiles реализует считывание нужного профиля из profiles.xml и сохранение изменений внесенных в профиль

Пример файла с двумя профилями пайки
<?xml version="1.0" encoding="UTF-8" ?>
<profiles>
	<profile id="0" title="down 140 up 200">
		<point type="down" value="100"></point>
		<point type="wait" value="30"></point>
		<point type="down" value="140"></point>
		<point type="wait" value="40"></point>
		<point type="up" value="200"></point>
	</profile>
	<profile id="1" title="preheat 100">
			<point type="down" value="100"></point>
	</profile>
</profiles>
*/
#ifndef PROFILE_H
#define PROFILE_H
#include "mbed.h"
#include "tinyxml.h"
#include <string>

/*
* ProfilePoint - список, который содержит шаги профиля пайки. type принимает значения up,down,wait
* up - использовать верхний нагреватель
* down - использовать нижний нагреватель
* wait - ожидать
* value - значение, означающее либо температуру в градусах цельсия для шага up или down
* или задержку в секундах для шага wait
* next - указатель на следующий шаг
* previous - указатель на предыдущий шаг
*/
struct ProfilePoint
{
    string type;
    int value;
    ProfilePoint *next=NULL;
    ProfilePoint *previous=NULL;
};

// Список всех профилей
struct Profile_Id_Title
{
    int id;
    string title;
    Profile_Id_Title *next=NULL;
    Profile_Id_Title *previous=NULL;
};


class Profiles
{
    ProfilePoint *points; // список с этапами пайки
    Profile_Id_Title *all_profiles;
    int PointsCount; // количество этапов пайки
    int ProfilesCount;
    bool fileLoaded; // true, если файл с профилями загружен удачно
    TiXmlDocument doc;// xml файл с профилями
    string title; // название выбранного профиля
    int profileId;// id выбранного профиля -1, если не выбран ни один профиль

    void ClearPoints(ProfilePoint *x);// удаляет из памяти список с этапами пайки

    public:
    Profiles();
    int init(); // загружает profiles.xml с флеш карты 1-удачно, 0 - неудачно
    int LoadProfiles();//загружажет профили в список Profile_Id_Title;
    int SelectProfile(int id); //сделать текущим профиль с номером id
    string GetProfileName(); //возвращает название выбранного профиля
    int GetCountOfPoints();// получить количество этапов пайки
    ProfilePoint *GetPoints();// получить список этапов пайки
    Profile_Id_Title *GetProfiles();// получить список профилей

};
#endif

