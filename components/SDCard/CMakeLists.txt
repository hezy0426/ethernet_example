idf_component_register(SRCS "dataLogging.cpp" "SDCard.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES "sdmmc" "fatfs" "vfs" "webServer")
