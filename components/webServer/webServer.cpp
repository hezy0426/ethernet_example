/**
 * 07/25/2023 - Ruizhe He, this is an example for http server on esp-idf.
 * Http server is automatically started when the ethernet with static ip
 * is setup. (See ethernet_example_main.cpp).
 *
 * Go to the ip address once everything is running, the sample page will return.
 * Currently, all the values are dummy values and stored in json_string.h, but the
 * timer works correctly and the all the basic functions should be running.
 *
 * HTML code is stored in the index_html.h. All the required JS and css files are stored
 * in myscript_js.h. Each required files for the html has its own handler.
 */

#include <stdio.h>
#include <string.h>
#include "webServer.h"
#include <esp_http_server.h>
#include "esp_err.h"
#include "esp_check.h"
#include <esp_event.h>
#include <esp_log.h>
#include "cJSON.h"
#include "index_html.h"
#include "myscript_js.h"
#include "json_string.h"

// A time for the timertick function in html
long long this_time = 1690495162000;

/**
 * This function isn't used ,but it can be used as an example
 *  to show how cJSON works.
 */
static char *buildJSONString(char *input)
{
    char *JSONString = NULL;
    cJSON *data = NULL;

    data = cJSON_CreateObject();

    cJSON_AddStringToObject(data, "output", (const char *)input);
    JSONString = cJSON_Print(data);
    cJSON_Delete(data);
    return JSONString;
}

/**
 * Since we don't have access to the time module, manually transfer a
 * time to the front-end for it to work with. Long long may not
 * work with cJSON since it exceeds INT_MAX, so it's saved as a string.
 */
static esp_err_t getTime_get_handler(httpd_req_t *req)
{
    // Allocate a character array to store the converted string
    char *baseString = "{\"data\":%lld}";
    int bufferSize = snprintf(NULL, 0, baseString, this_time) + 1;
    char buffer[bufferSize]; // Make sure the buffer is large enough to hold the string representation
    // Convert the long long value to a const char* using sprintf
    snprintf(buffer, bufferSize, baseString, this_time);
    // Increments by 10 seconds because the js functions checks if the time has changed
    this_time += 10000;
    const char *result = buffer;
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_send(req, result, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t getTime = {
    .uri = "/getTime",
    .method = HTTP_GET,
    .handler = getTime_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL};

/**
 * This will return channel json string. Channel seems to hold
 * definition and explaining info for data return in dataNow.json
 */
static esp_err_t channel_get_handler(httpd_req_t *req)
{

    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, channel_string, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t channel_handler = {
    .uri = "/channel.json",
    .method = HTTP_GET,
    .handler = channel_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL};

/**
 * This will return dataNow information. This should be generated in
 * real time in the actual project. This is the data shown under
 * "Now" table
 */
static esp_err_t dataNow_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, dataNow_string, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t dataNow = {
    .uri = "/dataNow.json",
    .method = HTTP_GET,
    .handler = dataNow_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL};

/**
 * This returns a css file for the html. Each external file requires a
 * separated handler.
 */
static esp_err_t magnum_style_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/css");
    return httpd_resp_send(req, magnum_css, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t magnumStyle = {
    .uri = "/magnum_style.css",
    .method = HTTP_GET,
    .handler = magnum_style_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return a required javascript file for html.
 */
static esp_err_t magnum_js_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, magnum_js, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t magnumjs = {
    .uri = "/magnum.js",
    .method = HTTP_GET,
    .handler = magnum_js_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return a required javascript file for html.
 */
static esp_err_t jquery_11_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, jquery_1_11, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t jquery_11 = {
    .uri = "/jquery-1.11.1.min.js",
    .method = HTTP_GET,
    .handler = jquery_11_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return a required javascript file for html.
 */
static esp_err_t excanas_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, excanvas, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t excanvasJS = {
    .uri = "/excanvas.min.js",
    .method = HTTP_GET,
    .handler = excanas_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return a required javascript file for html.
 */
static esp_err_t jquery_float_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, jqueryfloat, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t jquery_float = {
    .uri = "/jquery.flot.js",
    .method = HTTP_GET,
    .handler = jquery_float_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return a required javascript file for html.
 */
static esp_err_t jquery_threshold_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, jqery_threshold, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t jquery_threshold = {
    .uri = "/jquery.flot.threshold.js",
    .method = HTTP_GET,
    .handler = jquery_threshold_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return a required javascript file for html.
 */
static esp_err_t howler_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, howler, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t howler_handler = {
    .uri = "/howler.min.js",
    .method = HTTP_GET,
    .handler = howler_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return a required javascript file for html.
 */
static esp_err_t alarm_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, alarm, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t alarm_handler = {
    .uri = "/alarm.js",
    .method = HTTP_GET,
    .handler = alarm_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return a required javascript file for html.
 */
static esp_err_t date_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, date_js, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t date_handler = {
    .uri = "/date.js",
    .method = HTTP_GET,
    .handler = date_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

/**
 * This will return the host info. It contains
 * memory info of the SD. It's only dummy value here.
 */
static esp_err_t hostinfo_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, hostInfo_string, HTTPD_RESP_USE_STRLEN);
}
static const httpd_uri_t hostinfo_handler = {
    .uri = "/hostinfo.json",
    .method = HTTP_GET,
    .handler = hostinfo_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)""};

// This function will handle the ip/ request
// It will return a website
static esp_err_t indexPage_get_handler(httpd_req_t *req)
{

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    return httpd_resp_send(req, currentConditionPage, HTTPD_RESP_USE_STRLEN);
}

static const httpd_uri_t indexPage = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = indexPage_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL};

// Starting the server, if it fails to start the server, then returns NULL.
httpd_handle_t start_webserver()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 20;
    config.lru_purge_enable = true;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI("webServer", "WEB server started");
        httpd_register_uri_handler(server, &indexPage);
        httpd_register_uri_handler(server, &channel_handler);
        httpd_register_uri_handler(server, &magnumjs);
        httpd_register_uri_handler(server, &magnumStyle);
        httpd_register_uri_handler(server, &jquery_11);
        httpd_register_uri_handler(server, &excanvasJS);
        httpd_register_uri_handler(server, &jquery_float);
        httpd_register_uri_handler(server, &jquery_threshold);
        httpd_register_uri_handler(server, &howler_handler);
        httpd_register_uri_handler(server, &alarm_handler);
        httpd_register_uri_handler(server, &date_handler);
        httpd_register_uri_handler(server, &hostinfo_handler);
        httpd_register_uri_handler(server, &dataNow);
        httpd_register_uri_handler(server, &getTime);
        return server;
    }
    ESP_LOGI("webServer", "WEB failed to start");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    ESP_LOGI("webServer", "This server has stopped");
    httpd_stop(server);
}