#pragma once

#include "Data2Web.h"
#include "SDCard.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <cstdint>
#include <sys/stat.h>
#include <string>
#include <time.h>
#include <random>
#include <vector>

void startLogging();
void stopLogging();
void starterFunction();