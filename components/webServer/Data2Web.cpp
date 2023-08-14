/**
 * 07/28/2023 - Ruizhe He
 * Build a JSON string that contains all front end needed data from evo2Mnp.
 */

#include "Data2Web.h"

const int MAX_COUNT = 10;
std::map<const char *, historyObj> histMap;

/**
 * Template function to convert any type of int value to a string
 */
template <typename T>
const char *intToString(T value)
{
    std::string strValue = std::to_string(value);
    char *cstr = new char[strValue.length() + 1];
    std::strcpy(cstr, strValue.c_str());
    return cstr;
}

/**
 * Create a json object to be added into the json array object. This function
 * will store all the number value ones.
 */
template <typename T>
cJSON *createDataNowJSONObj(const char *channelData, const char *timeVal, T sampleVal)
{
    cJSON *dataHolder = NULL;
    dataHolder = cJSON_CreateObject();
    cJSON_AddStringToObject(dataHolder, "channel", (const char *const)channelData);
    cJSON_AddStringToObject(dataHolder, "time", (const char *const)timeVal);
    cJSON_AddNumberToObject(dataHolder, "avg", sampleVal);
    return dataHolder;
}

/**
 * This function will create a json object to be added into the json array.
 * In html code, all sampleValue will be parsed into the desired value, so
 * we can just store as string here.
 */
cJSON *createDataNowJSONObj2(const char *channelData, const char *timeVal, const char *sampleVal)
{
    cJSON *dataHolder = NULL;
    dataHolder = cJSON_CreateObject();
    cJSON_AddStringToObject(dataHolder, "channel", (const char *const)channelData);
    cJSON_AddStringToObject(dataHolder, "time", (const char *const)timeVal);
    cJSON_AddStringToObject(dataHolder, "sampleValue", (const char *const)sampleVal);
    return dataHolder;
}

/**
 * Create the json string for /dataNow.json request. It should contain
 * all the current info for the battery, inverter and AGS.
 *
 * FIXME: Eventually replace the dummy value with other things
 */
char *getDataNow(long long time)
{
    char *JSONString = NULL;
    cJSON *JSONObj = NULL;
    cJSON *data = NULL;
    const char *currentTime = intToString(time);
    std::mt19937_64 rng(time);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // This will hold the whole JSON data structure
    JSONObj = cJSON_CreateObject();
    cJSON_AddItemToObject(JSONObj, "data", data = cJSON_CreateArray());

    // Battery
    double volt = 25.0 + 5.0 * dist(rng);
    double amp = 1 + 1 * dist(rng);
    double watt = volt * amp;
    cJSON_AddItemToArray(data, createDataNowJSONObj("b_dc_volts", currentTime, volt));
    cJSON_AddItemToArray(data, createDataNowJSONObj("b_dc_amps", currentTime, amp));
    cJSON_AddItemToArray(data, createDataNowJSONObj("b_dc_watts", currentTime, watt)); // TODO: Needs to merge first
    cJSON_AddItemToArray(data, createDataNowJSONObj("b_state_of_charge", currentTime, 95 + 5 * dist(rng)));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("b_amph_in_out", currentTime, intToString(-1 + 1 * dist(rng))));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("age_bmk", currentTime, intToString(5.0))); // Dummy value
    cJSON_AddItemToArray(data, createDataNowJSONObj2("r_low_batt_cut_out", currentTime, "24.4"));

    // Inverter
    cJSON_AddItemToArray(data, createDataNowJSONObj2("i_model", currentTime, intToString(218)));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("i_status", currentTime, intToString(0)));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_temp_battery", currentTime, 34 + 3 * dist(rng)));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_temp_transformer", currentTime, 50 + 5 * dist(rng)));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_temp_fet", currentTime, 44));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_ac_volts_out", currentTime, 120 + 3 * dist(rng)));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_ac_volts_in", currentTime, 181));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_ac_hz", currentTime, 58 + 3 * dist(rng)));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_dc_amps", currentTime, 0));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_amps_out", currentTime, 0));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_amps_in", currentTime, 0));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("b_fault", currentTime, "1"));
    cJSON_AddItemToArray(data, createDataNowJSONObj("i_dc_volts", currentTime, 28));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("i_fault", currentTime, "0"));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("age_inverter", currentTime, intToString(1.0)));     // Dummy value
    cJSON_AddItemToArray(data, createDataNowJSONObj2("length_inverter", currentTime, intToString(22.0))); // Dummy value

    // AGS
    cJSON_AddItemToArray(data, createDataNowJSONObj2("a_status", currentTime, "1.0"));
    cJSON_AddItemToArray(data, createDataNowJSONObj("a_temperature", currentTime, 251));
    cJSON_AddItemToArray(data, createDataNowJSONObj("a_voltage", currentTime, 26 + 1 * dist(rng)));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("a_gen_runtime_decihours", currentTime, "1.5"));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("a_days_since_last", currentTime, "2"));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("a_gen_run_hours_since_boot", currentTime, "3"));
    cJSON_AddItemToArray(data, createDataNowJSONObj2("age_ags_0xA1", currentTime, intToString(9.0))); // Dummy value

    delete[] currentTime;
    JSONString = cJSON_Print(JSONObj);
    cJSON_Delete(JSONObj);
    return JSONString;
}

char *getHostInfoJson()
{
    char *JSONString = NULL;
    cJSON *JSONObj = NULL;
    cJSON *drives = NULL;
    cJSON *sd_card = NULL;
    uint32_t tot, free;
    bool ret;
    if (!isMounted())
        initi_sd_card();
    if (isMounted())
        ret = SD_getFreeSpace(&tot, &free);
    else
        tot = free = 0;

    ESP_LOGI("SD_CARD", "%i KiB total drive space. %i KiB available.", tot, free);
    JSONObj = cJSON_CreateObject();

    cJSON_AddStringToObject(JSONObj, "hostname", "RAYCING");
    cJSON_AddItemToObject(JSONObj, "drives", drives = cJSON_CreateArray());
    cJSON_AddItemToArray(drives, sd_card = cJSON_CreateObject());

    // SD card info
    cJSON_AddNumberToObject(sd_card, "total", tot);
    cJSON_AddNumberToObject(sd_card, "used", tot - free);
    cJSON_AddNumberToObject(sd_card, "avail", free);
    cJSON_AddStringToObject(sd_card, "description", "SD_Card");

    JSONString = cJSON_Print(JSONObj);
    cJSON_Delete(JSONObj);
    return JSONString;
}

cJSON *createDayStatObj(historyObj temp)
{
    cJSON *dataHolder = NULL;
    dataHolder = cJSON_CreateObject();
    cJSON_AddNumberToObject(dataHolder, "max", temp.max);
    cJSON_AddNumberToObject(dataHolder, "min", temp.min);
    cJSON_AddNumberToObject(dataHolder, "avg", temp.avg);
    return dataHolder;
}

char *getDaysStats()
{
    char *JSONString = NULL;
    cJSON *JSONObj = NULL;
    cJSON *dayStats = NULL;

    JSONObj = cJSON_CreateObject();
    cJSON_AddItemToObject(JSONObj, "dayStats", dayStats = cJSON_CreateObject());

    for (auto i = histMap.begin(); i != histMap.end(); i++)
    {
        cJSON_AddItemToObject(dayStats, i->first, createDayStatObj(i->second));
    }

    JSONString = cJSON_Print(JSONObj);
    cJSON_Delete(JSONObj);
    return JSONString;
}

/**
 * Before the vector size hits max number 10, we count the basic avg = sum/size
 */
void updateAverage(const char *valueName)
{
    historyObj *temp = &histMap[valueName];
    double sum = 0;
    for (auto i = temp->valueHolder.begin(); i != temp->valueHolder.end(); ++i)
    {
        sum += *i;
    };
    temp->avg = sum / temp->valueHolder.size();
}

/**
 * Once the vector size hits max number 10, we calculate the average with
 * rolling average. New value has been added to the vector at this point,
 * so it just needs to do: currentAvg + (newValue - oldestValue)/maxSize
 * After this, remove the oldest value so it can remain max size of 10.
 */
void updateRollingAvg(const char *valueName)
{
    historyObj *temp = &histMap[valueName];
    temp->avg = temp->avg + (temp->valueHolder[temp->valueHolder.size() - 1] - temp->valueHolder[0]) / MAX_COUNT;
    temp->valueHolder.erase(temp->valueHolder.begin());
}

/**
 * If this map key doesn't exist, create a new object and
 * insert it into the map. Since it's the first time being call,
 * all the value can be set with the firstValue.
 */
template <typename T>
void initMap(T firstValue, const char *valueName)
{
    historyObj hObj;
    hObj.avg = firstValue;
    hObj.max = firstValue;
    hObj.min = firstValue;
    hObj.valueHolder.push_back(firstValue);

    histMap.insert({valueName, hObj});
}

/**
 * If this key exists in the map already, update all values.
 */
void updateMap(double newValue, const char *valueName)
{
    historyObj *temp = &histMap[valueName];

    temp->valueHolder.push_back(newValue);

    if (temp->max < newValue)
        temp->max = newValue;
    else if (temp->min > newValue)
        temp->min = newValue;
    if (temp->valueHolder.size() < MAX_COUNT)
        updateAverage(valueName);
    else
    {
        updateRollingAvg(valueName);
    }
}

void addToRecent(std::vector<double> liveData)
{
    double i_ac_volts_out = liveData[0];
    double b_state_of_charge_clean = liveData[1];
    double b_dc_volts_clean = liveData[2];
    double b_dc_amps_clean = liveData[3];
    double b_dc_watts = liveData[4];
    double i_dc_volts = liveData[5];
    double i_dc_amps = liveData[6];
    double i_amps_out = liveData[7];
    double i_amps_in_clean = liveData[8];
    double i_ac_hz_clean = liveData[9];
    double i_temp_battery_clean = liveData[10];
    double i_temp_transformer_clean = liveData[11];
    double i_temp_fet_clean = liveData[12];
    double a_temperature_clean = liveData[13];
    double a_voltage_clean = liveData[14];

    int i_status = (int)liveData[15];

    if (i_ac_volts_out > 80)
    {
        double i_ac_volts_out_over_80 = i_ac_volts_out;
        if (histMap.count("i_ac_volts_out_over_80") == 0)
        {
            initMap(i_ac_volts_out_over_80, "i_ac_volts_out_over_80");
        }
        else
        {
            updateMap(i_ac_volts_out_over_80, "i_ac_volts_out_over_80");
        }
        ESP_LOGI("D2W", "Avg: %f, Min: %f, Max: %f, current: %f", histMap["i_ac_volts_out_over_80"].avg, histMap["i_ac_volts_out_over_80"].min, histMap["i_ac_volts_out_over_80"].max, i_ac_volts_out_over_80);
    }

    if (i_amps_out < 100 && 0x01 != i_status && 0x02 != i_status && 0x04 != i_status && 0x08 != i_status)
    {
        double i_amps_out_inverting = i_amps_out;

        if (histMap.count("i_amps_out_inverting") == 0)
        {
            initMap(i_amps_out_inverting, "i_amps_out_inverting");
        }
        else
        {
            updateMap(i_amps_out_inverting, "i_amps_out_inverting");
        }
    }

    if (i_amps_out < 100 && i_status >= 0x01 && i_status <= 0x08)
    {
        double i_amps_out_charging = i_amps_out;

        if (histMap.count("i_amps_out_charging") == 0)
        {
            initMap(i_amps_out_charging, "i_amps_out_charging");
        }
        else
        {
            updateMap(i_amps_out_charging, "i_amps_out_charging");
        }
    }

    // TODO: Assign input array value to local variable
    if (histMap.count("b_state_of_charge_clean") == 0)
    {
        initMap(b_state_of_charge_clean, "b_state_of_charge_clean");
    }
    else
    {
        updateMap(b_state_of_charge_clean, "b_state_of_charge_clean");
    }

    if (histMap.count("b_dc_volts_clean") == 0)
    {
        initMap(b_dc_volts_clean, "b_dc_volts_clean");
    }
    else
    {
        updateMap(b_dc_volts_clean, "b_dc_volts_clean");
    }

    if (histMap.count("b_dc_amps_clean") == 0)
    {
        initMap(b_dc_amps_clean, "b_dc_amps_clean");
    }
    else
    {
        updateMap(b_dc_amps_clean, "b_dc_amps_clean");
    }

    if (histMap.count("b_dc_watts") == 0)
    {
        initMap(b_dc_watts, "b_dc_watts");
    }
    else
    {
        updateMap(b_dc_watts, "b_dc_watts");
    }

    if (histMap.count("i_dc_volts") == 0)
    {
        initMap(i_dc_volts, "i_dc_volts");
    }
    else
    {
        updateMap(i_dc_volts, "i_dc_volts");
    }

    if (histMap.count("i_dc_amps") == 0)
    {
        initMap(i_dc_amps, "i_dc_amps");
    }
    else
    {
        updateMap(i_dc_amps, "i_dc_amps");
    }

    if (histMap.count("i_amps_in_clean") == 0)
    {
        initMap(i_amps_in_clean, "i_amps_in_clean");
    }
    else
    {
        updateMap(i_amps_in_clean, "i_amps_in_clean");
    }

    if (histMap.count("i_ac_hz_clean") == 0)
    {
        initMap(i_ac_hz_clean, "i_ac_hz_clean");
    }
    else
    {
        updateMap(i_ac_hz_clean, "i_ac_hz_clean");
    }

    if (histMap.count("i_temp_battery_clean") == 0)
    {
        initMap(i_temp_battery_clean, "i_temp_battery_clean");
    }
    else
    {
        updateMap(i_temp_battery_clean, "i_temp_battery_clean");
    }

    if (histMap.count("i_temp_transformer_clean") == 0)
    {
        initMap(i_temp_transformer_clean, "i_temp_transformer_clean");
    }
    else
    {
        updateMap(i_temp_transformer_clean, "i_temp_transformer_clean");
    }

    if (histMap.count("i_temp_fet_clean") == 0)
    {
        initMap(i_temp_fet_clean, "i_temp_fet_clean");
    }
    else
    {
        updateMap(i_temp_fet_clean, "i_temp_fet_clean");
    }

    if (histMap.count("a_temperature_clean") == 0)
    {
        initMap(a_temperature_clean, "a_temperature_clean");
    }
    else
    {
        updateMap(a_temperature_clean, "a_temperature_clean");
    }

    if (histMap.count("a_voltage_clean") == 0)
    {
        initMap(a_voltage_clean, "a_voltage_clean");
    }
    else
    {
        updateMap(a_voltage_clean, "a_voltage_clean");
    }
}