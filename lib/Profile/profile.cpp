#include "profile.h"

Profiles::Profiles()
{
    fileLoaded = false;
    profileId=-1;
    PointsCount = 0;
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
        do {
        
            if(!el->QueryIntAttribute("id", &x)==TIXML_SUCCESS) return 0;
            if (x==id) break;
            el = el->NextSiblingElement("profile");
        } while (el);
        if (x==-1) return 0;
        const char *ss= el->Attribute("title");
        title.append(ss);

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