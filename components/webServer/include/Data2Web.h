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

struct
{
    int counter;
    historyObj b_state_of_charge_clean;
    historyObj b_dc_volts_clean;
    historyObj b_dc_amps_clean;
    historyObj b_dc_watts;
    historyObj i_dc_volts;
    historyObj i_dc_amps;
    historyObj i_ac_volts_out_over_80;
    historyObj i_amps_out_inverting;
    historyObj i_amps_out_charging;
    historyObj i_amps_in_clean;
    historyObj i_ac_hz_clean;
    historyObj i_temp_battery_clean;
    historyObj i_temp_transformer_clean;
    historyObj i_temp_fet_clean;
    historyObj a_temperature_clean;
    historyObj a_voltage_clean;
} histObj;

char *getDataNow(long long time);
char *getHostInfoJson();
char *getDaysStats();
void addToRecent(std::vector<double> liveData);