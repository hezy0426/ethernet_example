#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdio.h>
#include <string.h>
#include "esp_http_server.h"
#include "esp_err.h"
#include "esp_check.h"
#include <esp_event.h>
#include <esp_log.h>

httpd_handle_t start_webserver();
void stop_webserver(httpd_handle_t server);
void connect_handler(void *arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data);
void disconnect_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);

#endif /* WEBSERVER_H */
