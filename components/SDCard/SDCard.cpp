#include <stdio.h>
#include "SDCard.h"
#include "esp_log.h"

#define MOUNT_POINT "/sdcard"

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

void testingCode(char *jsonString)
{
    if (!SDCARDMOUNTED)
        return;
    const char *file_hello = MOUNT_POINT "/hello.txt";

    ESP_LOGI(TAG_SD, "Opening file %s", file_hello);
    FILE *f = fopen(file_hello, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG_SD, "Failed to open file for writing");
        return;
    }
    fprintf(f, "The Data is:\n%s", jsonString);
    fclose(f);
    ESP_LOGI(TAG_SD, "SUCCESS");
}

bool isMounted(void)
{
    return SDCARDMOUNTED;
}

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