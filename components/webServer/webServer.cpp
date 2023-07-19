

#include <stdio.h>
#include <string.h>
#include "webServer.h"
#include <esp_http_server.h>
#include "esp_err.h"
#include "esp_check.h"
#include <esp_event.h>
#include <esp_log.h>

//HTML is separated for reusability
//Check this website for how to return different html https://esp32tutorials.com/esp32-web-server-esp-idf/
//TODO: Add the base url here, it will be the background of the standard website.
const char* BASEURL = "";
//TODO: Add the HTML code for the current condition.
const char* CURRENTCONDITION = "";
//Activate for future
//const char* CURRENTSETTING = "";
int counter = 0;

static const char *page1 = "<!DOCTYPE html>\n<html>\n<title>Online HTML Editor</title>\n\n<head>\n</head>\n\n<body>\n    <h1>Online HTML Editor</h1>\n    <div>This is real time online HTML Editor</div>\n    <p> <a href=\"/\"><button id=\\\"button1\\\">Current Condition</button></a> <a href=\"/setting\"><button id=\\\"button2\\\">Current Setting</button></a> </p>\n    <p>Data from http server, retrieves every 5 seconds</p>\n    <P id=\"counter\"></P>\n    <script>\n        function fetchData() {\n            var xhr = new XMLHttpRequest();\n            xhr.onreadystatechange = function() {\n                if (xhr.readyState === XMLHttpRequest.DONE) {\n                    if (xhr.status === 200) {\n                        document.getElementById('counter').innerText = xhr.responseText;\n                    } else {\n                        console.error('Error:', xhr.status);\n                    }\n                }\n            };\n            xhr.open('GET', '/data', true);\n            xhr.send();\n        }\n   fetchData();\n     setInterval(fetchData, 5000);\n    </script>\n</body>\n\n</html>\n\n<!DOCTYPE html>\n<html>";

static const char *setting = "<!DOCTYPE html>\n<html>\n<title>Online HTML Editor</title>\n\n<head>\n</head>\n\n<body>\n    <h1>Online HTML Editor</h1>\n    <p> <a href=\"/\"><button id=\\\"button1\\\">Current Condition</button></a> <a href=\"/setting\"><button id=\\\"button2\\\">Current Setting</button></a> </p>\n    <div>This is the setting page</div>\n</body>\n\n</html>";

//Return a data to update the website using ajax
static esp_err_t data_get_handler(httpd_req_t *req){
    char resp[30];
    sprintf(resp, "The number is: %d", counter++);
    httpd_resp_set_type(req,"text/plain text");
    return httpd_resp_send(req, (const char *) resp, strlen(resp));
}


static const httpd_uri_t data = {
    .uri = "/data",
    .method = HTTP_GET,
    .handler = data_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)"Returns data"};


//This function will handle the ip+/hello request
//It will return a website
static esp_err_t hello_get_handler(httpd_req_t *req)
{


    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");


    //TODO tomorrow, check if this works. Otherwise just convert html file to C header file. See examples below
    //https://www.esp32.com/viewtopic.php?t=7388   https://esp32.com/viewtopic.php?t=6966
    // int resultLength = strlen(BASEURL)+strlen(CURRENTCONDITION) - 2 + 1;
    // char result[resultLength];
    // snprintf(result, resultLength, BASEURL, CURRENTCONDITION);
    // httpd_resp_send(req, (const char *) result, resultLength);
    return httpd_resp_send(req,page1,HTTPD_RESP_USE_STRLEN);
}


static const httpd_uri_t hello = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)"<h1>Hello World!</h1>"};

static esp_err_t setting_get_handler(httpd_req_t *req)
{


    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");
    return httpd_resp_send(req,setting,HTTPD_RESP_USE_STRLEN);
}


static const httpd_uri_t setPage = {
    .uri = "/setting",
    .method = HTTP_GET,
    .handler = setting_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void*)""};


//Starting the server, if it fails to start the server, then returns NULL.
httpd_handle_t start_webserver()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI("webServer", "WEB server started");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &data);
        httpd_register_uri_handler(server, &setPage);
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