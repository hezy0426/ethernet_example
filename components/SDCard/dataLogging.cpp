#include "dataLogging.h"

#define HEADER1 "Data Date (UTC),Milliseconds,b_dc_power,i_dc_power,calc_add_power,gen_power,load_power,b_dc_volts,b_dc_amps,i_ac_volts_in,b_amph_in_out,b_state_of_charge,i_dc_volts,i_dc_amps,i_ac_volts_out,i_amps_out,i_amps_in,i_ac_hz,i_status,i_fault,i_temp_transformer,i_temp_fet,i_temp_battery,a_gen_run_hours_since_boot,a_gen_runtime_decihours,age_inverter,a_temperature"
#define HEADER2 "Data Date (UTC),Milliseconds,DC Power (RMK),DC Power (Inverter),Calculated Renewable Power,AC In Power,AC Out Power,VDC (BMK),DC Amps (BMK),VAC In (Inverter),Ah In/Out (BMK),SOC (BMK),VDC (Inveter),DC Amps (Inverter),VAC Out (Inverter),AC Amps Out,AC Amps IN,AC Out Hz,Inverter Status,Fault Code,Transformer Temp (C ),FET Temp (C ),Battery Temp (C ),Total Generator Runtime (Since AGS Boot),Generator Runtime (Current Cycle),Inverter Age (255 indicates old data),AGS Temp (C )"
#define HEADER_SIZE 840

time_t epoch_time = 1691547163;
const TickType_t xDelay = 10000;
TaskHandle_t task_handle;
#define TAG_DL "Data_Logging"

// int generateHeadlines(char **headlineA, char **headlineB)
// {
//     std::string ha, hb;
//     ha += "Data Date (UTC),Milliseconds";
//     hb += "Data Date (UTC),Milliseconds";
//     dataNowString = getDataNow();
//     cJSON *dataNowJson = cJSON_Parse(dataNowString);
//     cJSON *channelJson = cJSON_Parse(channel_string);

//     if (!dataNowJson || !channelJson)
//     {
//         cJSON_Delete(dataNowJson);
//         cJSON_Delete(channelJson);
//         return 0;
//     }

//     cJSON *dataNowObj = cJSON_GetObjectItem(dataNowJson, "data");
//     cJSON *channelObj = cJSON_GetObjectItem(channelJson, "data");

//     if (!dataNowObj || !channelObj)
//     {
//         cJSON_Delete(dataNowJson);
//         cJSON_Delete(channelJson);
//         return 0;
//     }

//     cJSON *dataNowSubObj = NULL;
//     cJSON *channelSubObj = NULL;
//     cJSON_ArrayForEach(dataNowSubObj, dataNowObj)
//     {
//         cJSON *channel = cJSON_GetObjectItemCaseSensitive(dataNowSubObj, "channel");
//         ha += channel->valueString;

//         cJSON_ArrayForEach(channelSubObj, channelObj)
//         {
//             cJSON *id = cJSON_GetObjectItemCaseSensitive(channelSubObj, "id");
//             if (strcmp(id->valuestring, channel->valuestring) == 0)
//             {
//                 cJSON *descriptionA = cJSON_GetObjectItemCaseSensitive(channelSubObj, "description");
//                 hb += descriptionA->valueString;
//                 break;
//             }
//         }
//     }

//     return 1;
// }

void dataNowLog(void *pv_args)
{
    std::vector<float> liveData;
    struct tm *currentTime;

    for (;;)
    {
        // Move into the loop so when a new day comes, it will generate a new file
        char fileName[15];
        char time_str[25];
        char buffer[130];
        char headers[840];

        currentTime = localtime(&epoch_time);
        strftime(fileName, sizeof fileName, "%Y%m%d.csv", currentTime);
        std::mt19937_64 rng(epoch_time);
        std::uniform_real_distribution<float> dist(0.0, 1.0);

        float i_ac_volts_out = 120 + 3 * dist(rng);
        float b_state_of_charge = 95 + 5 * dist(rng);
        float b_dc_volts = 25.0 + 5.0 * dist(rng);
        float b_dc_amps = 1 + 1 * dist(rng);
        float b_dc_watts = b_dc_volts * b_dc_amps;
        float i_dc_volts = 27.0 + 1 * dist(rng);
        float i_dc_amps = 0.0;
        float i_amps_out = 0;
        float i_amps_in = 0;
        float i_ac_hz = 58 + 3 * dist(rng);
        float i_temp_battery = 34 + 3 * dist(rng);
        float i_temp_transformer = 50 + 5 * dist(rng);
        float i_temp_fet = 44;
        float a_temperature = 240 + 5 * dist(rng);
        float a_voltage_clean = 26 + 1 * dist(rng);
        float b_amph_in_out = -1 + 1 * dist(rng);
        float a_gen_runtime_decihours = 1.5;
        float a_gen_run_hours_since_boot = 1.5;
        int i_status = 0;
        int i_fault = 0;
        float i_ac_volts_in = 181;

        liveData.push_back(i_ac_volts_out);
        liveData.push_back(b_state_of_charge);
        liveData.push_back(b_dc_volts);
        liveData.push_back(b_dc_amps);
        liveData.push_back(b_dc_watts);
        liveData.push_back(i_dc_volts);
        liveData.push_back(i_dc_amps);
        liveData.push_back(i_amps_out);
        liveData.push_back(i_amps_in);
        liveData.push_back(i_ac_hz);
        liveData.push_back(i_temp_battery);
        liveData.push_back(i_temp_transformer);
        liveData.push_back(i_temp_fet);
        liveData.push_back(a_temperature);
        liveData.push_back(a_voltage_clean);
        liveData.push_back(b_amph_in_out);
        liveData.push_back(i_status);

        // Create the file if it doesn't exist. Add the headers to the file
        if (!hasFile(fileName))
        {
            snprintf(headers, HEADER_SIZE, "%s\n%s", HEADER1, HEADER2);
            if (logStringToFile(headers, fileName))
                ESP_LOGI(TAG_DL, "Created a new file %s", headers);
        }

        strftime(time_str, sizeof(time_str), "%Y/%m/%d %H:%M:%S", currentTime);
        currentTime = NULL;

        snprintf(buffer, 120, "%s,%d,%.0f,%.0f,%.0f,%.0f,%.0f,%.2f,%.2f,%.0f,%.0f,%.0f,%.2f,%.0f,%.0f,%.0f,%.0f,%.2f,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.0f",
                 time_str,
                 0, // Millisecond
                 b_dc_watts,
                 i_dc_amps * i_dc_volts,
                 b_dc_watts - i_dc_amps * i_dc_volts,
                 i_ac_volts_in * i_amps_in,
                 i_ac_volts_out * i_amps_out,
                 b_dc_volts,
                 b_dc_amps,
                 i_ac_volts_in,
                 b_amph_in_out,
                 b_state_of_charge,
                 i_dc_volts,
                 i_dc_amps,
                 i_ac_volts_out,
                 i_amps_out,
                 i_amps_in,
                 i_ac_hz,
                 i_status,
                 i_fault,
                 i_temp_transformer,
                 i_temp_fet,
                 i_temp_battery,
                 a_gen_run_hours_since_boot,
                 a_gen_runtime_decihours,
                 1, // age_inverter
                 a_temperature);
        ESP_LOGI(TAG_DL, "Logging this: %s", buffer);

        if (logStringToFile(buffer, fileName))
        {
            ESP_LOGI(TAG_DL, "Successfully did one");
        }
        addToRecent(liveData);
        liveData.clear();
        epoch_time += 10;
        // TODO: Need the format of the file
        vTaskDelay(xDelay);
    }
}

/**
 * This function will mount the SD card and then start the logging of live data.
 */
void startLogging()
{
    if (!isMounted())
    {
        esp_err_t mounted = initi_sd_card();
        if (mounted != ESP_OK)
        {
            ESP_LOGE(TAG_DL, "SD card failed to mount, logging task didn't start");
            return;
        }

        BaseType_t ret = xTaskCreate(dataNowLog, "dataNow_Logging_Task", 8192, NULL, 5, &task_handle);
        if (ret == pdPASS)
        {
            ESP_LOGI(TAG_DL, "Successfully started the task");
        }
    }
}

/**
 * Stop the logging of live data when the function is called.
 */
void stopLogging()
{
    if (task_handle)
    {
        vTaskDelete(task_handle);
        task_handle = NULL;
    }
}

void starterFunction()
{
    startLogging();
}