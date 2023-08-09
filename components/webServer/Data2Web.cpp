/**
 * 07/28/2023 - Ruizhe He
 * Build a JSON string that contains all front end needed data from evo2Mnp.
 */

#include "Data2Web.h"
#include "SDCard.h"

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
    cJSON_AddItemToArray(data, createDataNowJSONObj2("b_amph_in_out", currentTime, intToString(0.0)));
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

// char *getDaysStats()
// {
//     return NULL;
// }