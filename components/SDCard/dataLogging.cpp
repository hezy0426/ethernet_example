#include "dataLogging.h"

#define HEADER1 "Data Date (UTC),Milliseconds,b_dc_power,i_dc_power,calc_add_power,gen_power,load_power,b_dc_volts,b_dc_amps,i_ac_volts_in,b_amph_in_out,b_state_of_charge,i_dc_volts,i_dc_amps,i_ac_volts_out,i_amps_out,i_amps_in,i_ac_hz,i_status,i_fault,i_temp_transformer,i_temp_fet,i_temp_battery,a_gen_run_hours_since_boot,a_gen_runtime_decihours,age_inverter,a_temperature"
#define HEADER2 "Data Date (UTC),Milliseconds,DC Power (RMK),DC Power (Inverter),Calculated Renewable Power,AC In Power,AC Out Power,VDC (BMK),DC Amps (BMK),VAC In (Inverter),Ah In/Out (BMK),SOC (BMK),VDC (Inveter),DC Amps (Inverter),VAC Out (Inverter),AC Amps Out,AC Amps IN,AC Out Hz,Inverter Status,Fault Code,Transformer Temp (C ),FET Temp (C ),Battery Temp (C ),Total Generator Runtime (Since AGS Boot),Generator Runtime (Current Cycle),Inverter Age (255 indicates old data),AGS Temp (C )"

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
    std::string tempString;
    std::vector<double> liveData;
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

        double i_ac_volts_out = 120 + 3 * dist(rng);
        double b_state_of_charge = 95 + 5 * dist(rng);
        double b_dc_volts = 25.0 + 5.0 * dist(rng);
        double b_dc_amps = 1 + 1 * dist(rng);
        double b_dc_watts = b_dc_volts * b_dc_amps;
        double i_dc_volts = 27.0 + 1 * dist(rng);
        double i_dc_amps = 0.0;
        double i_amps_out = 0;
        double i_amps_in = 0;
        double i_ac_hz = 58 + 3 * dist(rng);
        double i_temp_battery = 34 + 3 * dist(rng);
        double i_temp_transformer = 50 + 5 * dist(rng);
        double i_temp_fet = 44;
        double a_temperature = 240 + 5 * dist(rng);
        double a_voltage_clean = 26 + 1 * dist(rng);
        double b_amph_in_out = -1 + 1 * dist(rng);
        double a_gen_runtime_decihours = 1.5;
        double a_gen_run_hours_since_boot = 1.5;
        double i_status = 0;
        int i_ac_volts_in = 181;

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
        liveData.push_back(0);

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
        tempString.append(std::to_string(b_dc_watts)).append(",");
        // i_dc_power
        tempString.append(std::to_string(i_dc_amps * i_dc_volts)).append(",");
        // calc_add_power
        tempString.append(std::to_string(b_dc_watts - i_dc_amps * i_dc_volts)).append(",");
        // gen_power
        tempString.append(std::to_string(i_ac_volts_in * i_amps_in)).append(",");
        // load_power
        tempString.append(std::to_string(i_ac_volts_out * i_amps_out)).append(",");
        // b_dc_volts
        tempString.append(std::to_string(b_dc_volts)).append(",");
        // b_dc_amps
        tempString.append(std::to_string(b_dc_amps)).append(",");
        // i_ac_volts_in
        tempString.append(std::to_string(i_ac_volts_in)).append(",");
        // b_amph_in_out
        tempString.append(std::to_string(b_amph_in_out)).append(",");
        // b_state_of_charge
        tempString.append(std::to_string(b_state_of_charge)).append(",");
        // i_dc_volts
        tempString.append(std::to_string(i_dc_volts)).append(",");
        // i_dc_amps
        tempString.append(std::to_string(i_dc_amps)).append(",");
        // i_ac_volts_out
        tempString.append(std::to_string(i_ac_volts_out)).append(",");
        // i_amps_out
        tempString.append(std::to_string(i_amps_out)).append(",");
        // i_amps_in
        tempString.append(std::to_string(i_amps_in)).append(",");
        // i_ac_hz
        tempString.append(std::to_string(i_ac_hz)).append(",");
        // i_status
        tempString.append(std::to_string(i_status)).append(",");
        // i_fault
        tempString.append(std::to_string(0)).append(",");
        // i_temp_transformer
        tempString.append(std::to_string(i_temp_transformer)).append(",");
        // i_temp_fet
        tempString.append(std::to_string(i_temp_fet)).append(",");
        // i_temp_battery
        tempString.append(std::to_string(i_temp_battery)).append(",");
        // a_gen_run_hours_since_boot
        tempString.append(std::to_string(a_gen_run_hours_since_boot)).append(",");
        // a_gen_runtime_decihours
        tempString.append(std::to_string(a_gen_runtime_decihours)).append(",");
        // age_inverter         a_temperature
        tempString.append("1,").append(std::to_string(a_temperature));

        if (logStringToFile(tempString.c_str(), fileName))
        {
            ESP_LOGI(TAG_DL, "Successfully did one");
        }
        tempString.clear();
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