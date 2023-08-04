#pragma once

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdio.h>
#include <string.h>
#include "esp_http_server.h"
// List of constants
#define ERR_INVALID_ARG "Null argument(s)"
#define ERR_HTTP_ALLOC_MEM "Failed to allocate memory for instance"
#define ERR_HTTP_TASK "Failed to launch server task"
const char *const TAG_WEB = "Web_Server";

httpd_handle_t start_webserver();
void stop_webserver(httpd_handle_t server);

#endif /* WEBSERVER_H */
