/**
 * Http Server
 *
 * 07/17/2023 Ruizhe He
 * Currently, this file is still WIP, the full website html code is still missing.
 * The web server starting and stopping functions are implemented, but the web
 * server doesn't start here and that is done in EthernetConnect.cpp.
 * Basic usage:
 * 1. Create a hanlder function
 * 2. Create a URI handler
 * 3. Register the handler in start_webserver()
 */

#include "WebServer.h"
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_event.h"
#include "esp_check.h"
#include <esp_event.h>
#include <esp_log.h>
#include "Data2Web.h"

// All the CSS, HTML, and JS code stored in const char*
#include "myscript_js.h"
#include "css.h"
#include "index_html.h"
#include "json_string.h"

// A time for the timertick function in html
long long this_time = 1690495162000;

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
 * Battery chart and amp in/out need some data to start with. This
 * is just some dummy data to jump start both charts.
 */
static esp_err_t chartData_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, chartJumpStart_String, HTTPD_RESP_USE_STRLEN);
}
static const httpd_uri_t chartData = {
    .uri = "/recent.json",
    .method = HTTP_GET,
    .handler = chartData_get_handler,
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
    char *dataNowString = getDataNow(this_time);
    this_time += 10000;
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, dataNowString, HTTPD_RESP_USE_STRLEN);
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
    return httpd_resp_send(req, alarm_js, HTTPD_RESP_USE_STRLEN);
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
    const char *hostInfo = getHostInfoJson();
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, hostInfo, HTTPD_RESP_USE_STRLEN);
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
    config.stack_size = 20480;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG_WEB, "WEB server started");
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
        httpd_register_uri_handler(server, &chartData);
        return server;
    }
    ESP_LOGI(TAG_WEB, "WEB failed to start");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    ESP_LOGI(TAG_WEB, "This server has stopped");
    httpd_stop(server);
}