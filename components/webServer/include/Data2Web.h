#pragma once

#include "cJSON.h"
#include <cstdint>
#include <string>
#include <cstring>
#include <time.h>
#include <random>

char *getDataNow(long long time);
char *getHostInfoJson();