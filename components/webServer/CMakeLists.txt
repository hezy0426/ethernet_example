idf_component_register(SRCS "webServer.cpp"
                        REQUIRES "esp_http_server" "json"

                    INCLUDE_DIRS "include")
    