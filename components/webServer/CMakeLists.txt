idf_component_register(SRCS "file_server.cpp" "Data2Web.cpp" "webServer.cpp"
                        REQUIRES "esp_http_server" "json" "SDCard"

                    INCLUDE_DIRS "include")
    