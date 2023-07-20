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

#endif /* WEBSERVER_H */
