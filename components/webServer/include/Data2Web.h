#pragma once

#include "cJSON.h"
#include <cstdint>
#include <string>
#include <cstring>
#include <time.h>
#include <random>
#include <vector>
#include <map>

#include "SDCard.h"

class historyObj
{
public:
    double max;
    double avg;
    double min;
    std::vector<double> valueHolder;
};

char *getDataNow(long long time);
char *getHostInfoJson();
char *getDaysStats();
void addToRecent(std::vector<double> liveData);