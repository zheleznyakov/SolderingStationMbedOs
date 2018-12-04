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
};

class Profiles
{
    ProfilePoint *points;
    bool fileLoaded;
    TiXmlDocument doc;
    string title;
    int profileId;
    public:
    Profiles();
    int init();
    int SelectProfile(int id);
    string GetProfileName();

};
#endif

