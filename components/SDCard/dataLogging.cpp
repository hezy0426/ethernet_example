#include "dataLogging.h"

#define HEADER1 "Data Date (UTC),Milliseconds,b_dc_power,i_dc_power,calc_add_power,gen_power,load_power,b_dc_volts,b_dc_amps,i_ac_volts_in,b_amph_in_out,b_state_of_charge,i_dc_volts,i_dc_amps,i_ac_volts_out,i_amps_out,i_amps_in,i_ac_hz,i_status,i_fault,i_temp_transformer,i_temp_fet,i_temp_battery,a_gen_run_hours_since_boot,a_gen_runtime_decihours,age_inverter,a_temperature"
#define HEADER2 "Data Date (UTC),Milliseconds,DC Power (RMK),DC Power (Inverter),Calculated Renewable Power,AC In Power,AC Out Power,VDC (BMK),DC Amps (BMK),VAC In (Inverter),Ah In/Out (BMK),SOC (BMK),VDC (Inveter),DC Amps (Inverter),VAC Out (Inverter),AC Amps Out,AC Amps IN,AC Out Hz,Inverter Status,Fault Code,Transformer Temp (C ),FET Temp (C ),Battery Temp (C ),Total Generator Runtime (Since AGS Boot),Generator Runtime (Current Cycle),Inverter Age (255 indicates old data),AGS Temp (C )"

time_t epoch_time = 1691547163;
const TickType_t xDelay = 10000;
TaskHandle_t task_handle;

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
    std::string tempString;
    struct tm *currentTime;

    for (;;)
    {
        // Move into the loop so when a new day comes, it will generate a new file
        char fileName[15];
        char time_str[25];
        currentTime = localtime(&epoch_time);
        strftime(fileName, sizeof fileName, "%Y%m%d.csv", currentTime);
        std::mt19937_64 rng(epoch_time);
        std::uniform_real_distribution<double> dist(0.0, 1.0);

        // Create the file if it doesn't exist. Add the headers to the file
        if (!hasFile(fileName))
        {
            std::string headers;
            headers.append(HEADER1).append("\n").append(HEADER2);
            if (logStringToFile(headers.c_str(), fileName))
                ;
        }

        strftime(time_str, sizeof(time_str), "%Y/%m/%d %H:%M:%S", currentTime);
        currentTime = NULL;
        tempString.append(time_str).append(",0,");
        // b_dc_power
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        // i_dc_power
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        // calc_add_power
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        // gen_power
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        // load_power
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        tempString.append(std::to_string(25.0 + 5.0 * dist(rng))).append(",");
        // age_inverter
        tempString.append("1,").append(std::to_string(25.0 + 5.0 * dist(rng)));

        if (logStringToFile(tempString.c_str(), fileName))
        {
            ESP_LOGI("Logging", "Successfully did one");
        }
        tempString.clear();
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
        initi_sd_card();
        BaseType_t ret = xTaskCreate(dataNowLog, "dataNow_Logging_Task", 8192, NULL, 5, &task_handle);
        if (ret == pdPASS)
        {
            ESP_LOGI("Logging", "Successfully started the task");
        }
    }
    // TODO: confirm the priority with BA
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