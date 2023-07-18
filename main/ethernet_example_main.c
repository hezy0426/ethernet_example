/* Ethernet Basic Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "netdb.h"
#include <esp_http_server.h>
#include "nvs_flash.h"


#if CONFIG_ETH_USE_SPI_ETHERNET
#include "driver/spi_master.h"
#endif // CONFIG_ETH_USE_SPI_ETHERNET

static const char *TAG = "eth_example";
//HTML is separated for reusability
//Check this website for how to return different html https://esp32tutorials.com/esp32-web-server-esp-idf/
//TODO: Add the base url here, it will be the background of the standard website.
const char* BASEURL = ""; 
//TODO: Add the HTML code for the current condition. 
const char* CURRENTCONDITION = "";
//Activate for future
//const char* CURRENTSETTING = "";
int counter = 0; 

esp_netif_t *eth_netif;

#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
#define INIT_SPI_ETH_MODULE_CONFIG(eth_module_config, num)                                      \
    do {                                                                                        \
        eth_module_config[num].spi_cs_gpio = CONFIG_EXAMPLE_ETH_SPI_CS ##num## _GPIO;           \
        eth_module_config[num].int_gpio = CONFIG_EXAMPLE_ETH_SPI_INT ##num## _GPIO;             \
        eth_module_config[num].phy_reset_gpio = CONFIG_EXAMPLE_ETH_SPI_PHY_RST ##num## _GPIO;   \
        eth_module_config[num].phy_addr = CONFIG_EXAMPLE_ETH_SPI_PHY_ADDR ##num;                \
    } while(0)

    typedef struct {
    uint8_t spi_cs_gpio;
    uint8_t int_gpio;
    int8_t phy_reset_gpio;
    uint8_t phy_addr;
}spi_eth_module_config_t;
#endif

static void example_set_static_ip()
{
    esp_err_t ret;
    ret = esp_netif_dhcpc_stop(eth_netif);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to stop dhcp client:%d (%s)", ret, esp_err_to_name(ret));
        return;
        }
    esp_netif_ip_info_t ip;
    ESP_LOGI(TAG, "memset...");
    memset(&ip, 0, sizeof(esp_netif_ip_info_t));
    ip.ip.addr = ipaddr_addr("192.168.1.200");
    ip.netmask.addr = ipaddr_addr("255.255.255.0");
    ip.gw.addr = ipaddr_addr("192.168.1.1");
    ESP_LOGI(TAG, "esp_netif_set_ip_info()...");
    ret = esp_netif_set_ip_info(eth_netif, &ip);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set ip info, err:%d (%s)", ret, esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "Success to set static ip: %s, netmask: %s, gw: %s", "192.168.1.200", "255.255.255.0", "192.168.1.1");
    // ESP_ERROR_CHECK(example_set_dns_server(netif, ipaddr_addr(EXAMPLE_MAIN_DNS_SERVER), ESP_NETIF_DNS_MAIN));
    // ESP_ERROR_CHECK(example_set_dns_server(netif, ipaddr_addr(EXAMPLE_BACKUP_DNS_SERVER), ESP_NETIF_DNS_BACKUP));
}

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        ESP_LOGI(TAG, "Setting static IP");
        example_set_static_ip(eth_netif);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started.");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
}

//Return a data to update the website using ajax
static esp_err_t data_get_handler(httpd_req_t *req){
    char resp[30];
    sprintf(string, "The number is: %d", counter++);
    httpd_resp_set_type(req,"text/plain text");
	return httpd_resp_send(req, (const char *) resp, strlen(resp));
}

static const httpd_uri_t hello = {
	.uri = "/data",
	.method = HTTP_GET,
	.handler = data_get_handler,
	/* Let's pass response string in user
	 * context to demonstrate it's usage */
	.user_ctx = ""};

//This function will handle the ip+/hello request
//It will return a website
static esp_err_t hello_get_handler(httpd_req_t *req)
{

	/* Set some custom headers */
	httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
	httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    //TODO tomorrow, check if this works. Otherwise just convert html file to C header file. See examples below 
    //https://www.esp32.com/viewtopic.php?t=7388   https://esp32.com/viewtopic.php?t=6966
    int resultLength = strlen(BASEURL)+strlen(CURRENTCONDITION) - 2 + 1;
    char result[resultLength];
    snprintf(result, resultLength, BASEURL, CURRENTCONDITION);
    httpd_resp_set_type(req,"text/html");
	httpd_resp_send(req, (const char *) result, resultLength);

	/* After sending the HTTP response the old HTTP request
	 * headers are lost. Check if HTTP request headers can be read now. */
	if (httpd_req_get_hdr_value_len(req, "Host") == 0)
	{
		ESP_LOGI(LIB_TAG, "Request headers lost");
	}
	return ESP_OK;
}

static const httpd_uri_t hello = {
	.uri = "/hello",
	.method = HTTP_GET,
	.handler = hello_get_handler,
	/* Let's pass response string in user
	 * context to demonstrate it's usage */
	.user_ctx = (void *)"<h1>Hello World!</h1>"};


//Starting the server, if failed to start the server, then returns NULL. 
static httpd_handle_t start_webserver(void)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.lru_purge_enable = true;

	// Start the httpd server
	ESP_LOGI(LIB_TAG, "Starting server on port: '%d'", config.server_port);
	if (httpd_start(&server, &config) == ESP_OK)
	{
		// Set URI handlers
		ESP_LOGI(LIB_TAG, "Registering URI handlers");
		httpd_register_uri_handler(server, &hello);
		return server;
	}

	ESP_LOGI(LIB_TAG, "Error starting server!");
	return NULL;
}

//Set up before starting the server
static httpd_handle_t web_begin() 
{
	static httpd_handle_t server = NULL;

	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_netif_init());

	/* Start the server for the first time */
	server = start_webserver(); 
	return server;
}

void app_main(void)
{
    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());

// #if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET
    // Create new default instance of esp-netif for Ethernet
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    eth_netif = esp_netif_new(&cfg);

    // Init MAC and PHY configs to default
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;
    mac_config.smi_mdc_gpio_num = CONFIG_EXAMPLE_ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = CONFIG_EXAMPLE_ETH_MDIO_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
//#elif CONFIG_EXAMPLE_ETH_PHY_LAN87XX
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
    /* attach Ethernet driver to TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));

    httpd_handle_t server = web_begin();
    if (server == NULL)
        ESP_LOGD(TAG, "web server failed to start");
    else
        ESP_LOGD("Eth","Web server started successfully");
}
