/**
 * 08/04/2023 - Ruizhe He, this source file contains code to mount and unmount
 * a SD card from the ESP 32 board.
 */
#include "SDCard.h"

#define MOUNT_POINT "/sdcard"

/**
 * This function will mount the SD card
 */
esp_err_t initi_sd_card(void)
{
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_PROBING;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false, // Format the SD card if failed to mount
        .max_files = 3                   // Max amount of files opening at one time
    };

    esp_err_t err = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (err == ESP_OK)
    {
        SDCARDMOUNTED = true;
        ESP_LOGI(TAG_SD, "Mounted");
    }
    return err;
}

/**
 * This function will unmount the sd card
 */
esp_err_t unmount_sd_card(void)
{
    esp_err_t err = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);

    if (err == ESP_OK)
    {
        SDCARDMOUNTED = false;
        ESP_LOGI(TAG_SD, "Not Mounted");
    }

    return err;
}

/**
 * This function will write the parameter string into a txt file
 */
int logStringToFile(const char *formattedString, char *fileName)
{
    if (!SDCARDMOUNTED)
        return 0;

    std::string fullPath;
    fullPath.append(MOUNT_POINT).append("/").append(fileName);
    ESP_LOGI(TAG_SD, "Opening file %s", fullPath.c_str());

    FILE *f = fopen(fullPath.c_str(), "a");
    if (f == NULL)
    {
        ESP_LOGE(TAG_SD, "Failed to open file for writing");
        return 0;
    }
    fprintf(f, "%s\n", formattedString);
    fclose(f);
    ESP_LOGI(TAG_SD, "SUCCESS");
    return 1;
}

/**
 * Retrun a bool value whether the SD card is mounted or not
 */
bool isMounted(void)
{
    return SDCARDMOUNTED;
}

/**
 * This function will return a bool value whether we retrieved the value
 * successfully or not. Two parameters will be used to pass in and out
 * the free space and total space.
 */
bool SD_getFreeSpace(uint32_t *tot, uint32_t *free)
{
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;

    /* Get volume information and free clusters of drive 0 */
    if (f_getfree("0:", &fre_clust, &fs) == FR_OK)
    {
        /* Get total sectors and free sectors */
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;

        *tot = tot_sect / 2;
        *free = fre_sect / 2;

        /* Print the free space (assuming 512 bytes/sector) */
        ESP_LOGD("SD_CARD", "%i KiB total drive space. %i KiB available.", *tot, *free);

        return true;
    }
    return false;
}

int hasFile(char *fileName)
{
    struct stat st;
    std::string filePath;
    filePath.append(MOUNT_POINT).append("/").append(fileName);
    if (stat(filePath.c_str(), &st) == 0)
        return 1;
    return 0;
}