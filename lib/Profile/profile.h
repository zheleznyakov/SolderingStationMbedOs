#ifndef PROFILE_H
#define PROFILE_H
#include "mbed.h"
#include "tinyxml.h"
#include <string>

struct ProfilePoint
{
    string type;
    int value;
    ProfilePoint *next=NULL;
    ProfilePoint *previous=NULL;
};

class Profiles
{
    ProfilePoint *points;
    int PointsCount;
    bool fileLoaded;
    TiXmlDocument doc;
    string title;
    int profileId;

    void ClearPoints(ProfilePoint *x);

    public:
    Profiles();
    int init();
    int SelectProfile(int id);
    string GetProfileName();
    int GetCountOfPoints();
    ProfilePoint *GetPoints();

};
#endif

