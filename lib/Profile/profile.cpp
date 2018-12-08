/*
Пример xml файла с двумя профилями пайки
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
#include "profile.h"

Profiles::Profiles()
{
    fileLoaded = false;
    profileId=-1;
    PointsCount = 0;
    ProfilesCount =0;
    points = NULL;
}

int Profiles::init()
{
    if(!doc.LoadFile("/fs/profiles.xml"))
    {
        return 0;
    }
    fileLoaded = true;
    return 1;

}
int Profiles::LoadProfiles()
{
    if (fileLoaded)
    {
        TiXmlElement *root = doc.RootElement();
        Profile_Id_Title *p;
        p= new Profile_Id_Title();
        all_profiles = p;
        for (TiXmlElement *y= root->FirstChildElement("profile");y;y=y->NextSiblingElement("profile"))
        {
            if (ProfilesCount==0)
            {
                y->QueryIntAttribute("id",&p->id);
                p->title.append(y->Attribute("title"));
                
            }
            else{
                p->next = new Profile_Id_Title();
                p->next->previous = p;
                p=p->next;
                y->QueryIntAttribute("id",&p->id);
                p->title.append(y->Attribute("title"));

            }
            ProfilesCount++;

        }
        if (ProfilesCount>0) return 1;

    }
    return 0;
}
/*
* SelectProfile(int id) выбирает профиль из xml файла, возвращает 1 в случае удачи, 0 - неудача
* записывает название профиля в переменную title
*/
int Profiles::SelectProfile(int id)
{
    int x=-1;
    if (fileLoaded) 
    {
        TiXmlElement *el = doc.RootElement()->FirstChildElement("profile"); //находим первый профиль
        if (!el) return 0;
        do { // ищем нужный профиль, сравнивая id
        
            if(!el->QueryIntAttribute("id", &x)==TIXML_SUCCESS) return 0;
            if (x==id) break;
            el = el->NextSiblingElement("profile");
        } while (el);
        if (x==-1) return 0; //профиль с нужным id не найден
        const char *ss= el->Attribute("title");
        title.append(ss); 

        //Дальше будем считывать точки(этапы) профиля в список points

        ProfilePoint *p;
        
        p = new ProfilePoint();
        points = p;
        for (TiXmlElement *y= el->FirstChildElement("point");y;y=y->NextSiblingElement("point"))
        {
            if (PointsCount==0)
            {
                p->type =y->Attribute("type");
                y->QueryIntAttribute("value",&p->value);
            }
            else{

                p->next = new ProfilePoint();
                p->next->previous = p;
                p=p->next;

                p->type =y->Attribute("type");
                y->QueryIntAttribute("value",&p->value);
            }
            PointsCount++;
        }
        profileId=id;
        return 1;
    }
    return 0;
    

}
// ClearPoints - рекурсивная функция, удаляющая список с этапами пайки из памяти
void Profiles::ClearPoints(ProfilePoint *x)
{
    if (x==NULL) return;
    ClearPoints(x->next);
    delete x;
    x=NULL;
}
string Profiles::GetProfileName()
{
    return title;
}
int Profiles::GetCountOfPoints()
{
    return PointsCount;
}
ProfilePoint* Profiles::GetPoints()
{
    return points;
}
Profile_Id_Title* Profiles::GetProfiles()
{
    return all_profiles;
}