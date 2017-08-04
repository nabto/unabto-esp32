#include <stdio.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "demo_application.h"


#include <unabto/unabto_common_main.h>
#include <unabto/unabto_app.h>

// Set in menuconfig .. or override here
#define WIFI_SSID CONFIG_SSID
#define WIFI_PASS CONFIG_SSID_PASSWORD
#define NABTO_ID CONFIG_NABTO_ID
#define NABTO_KEY CONFIG_NABTO_KEY


// Event group
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;


/*
 * WIFI event handler
 */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch(event->event_id) {
		
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    break;
    
  case SYSTEM_EVENT_STA_GOT_IP:
    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    break;
    
  case SYSTEM_EVENT_STA_DISCONNECTED:
    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    break;
    
  default:
    break;
  }
  
  return ESP_OK;
}

int hctoi(const unsigned char h) {
  if (isdigit(h)) {
    return h - '0';
  } else {
    return toupper(h) - 'A' + 10;
  }
}


/*
 * Main task - initialize WIFI and start Nabto tick
 */
void main_task(void *pvParameter)
{
  // device id and key from developer.nabto.com
  const char* nabtoId = NABTO_ID;
  const char* presharedKey = NABTO_KEY;
  
  // wait for connection
  NABTO_LOG_INFO(("Main task: waiting for connection to the wifi network... "));
  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
  NABTO_LOG_INFO(("connected!\n"));
  
  // print the local IP address
  tcpip_adapter_ip_info_t ip_info;
  ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
  
  NABTO_LOG_INFO(("IP Address:  %s", ip4addr_ntoa(&ip_info.ip)));
  NABTO_LOG_INFO(("Subnet mask: %s", ip4addr_ntoa(&ip_info.netmask)));
  NABTO_LOG_INFO(("Gateway:     %s", ip4addr_ntoa(&ip_info.gw)));

  // Configure Nabto with the right configuration
  nabto_main_setup* nms = unabto_init_context();
  nms->id = NABTO_ID;
  nms->ipAddress = ip_info.ip.addr;
  nms->id = nabtoId;
  nms->secureAttach = 1;
  nms->secureData = 1;
  nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;

  const char* p;
  unsigned char* up;
  for (p = presharedKey, up = nms->presharedKey; *p; p += 2, ++up) {
    *up = hctoi(p[0]) * 16 + hctoi(p[1]);  // hex string to byte array
  }

  unabto_init();


  // Init demo application


  demo_init();
  
  demo_application_set_device_name("ESP32");
  demo_application_set_device_product("ACME 9002 Heatpump");
  demo_application_set_device_icon_("img/chip-small.png");



  while(true) {
    // Depending on the granuality of the freertos task switcher.. this is not very exact
    // But Nabto ticks doesn't need to be super exact
    vTaskDelay(10 / portTICK_RATE_MS); 
    
    unabto_tick();
  }
  

}

/**
 * Application event function for incomming Nabto req/res events
application_event_result application_event(application_request* request, unabto_query_request* read_buffer, unabto_query_response* write_buffer) {
    return AER_REQ_INV_QUERY_ID;
  }
 */


// Main application
void app_main()
{

 
  // disable the default wifi logging
  // esp_log_level_set("wifi", ESP_LOG_NONE);
  
  // disable stdout buffering
  setvbuf(stdout, NULL, _IONBF, 0);
 
 NABTO_LOG_INFO(("Nabto ESP32 demo starting up!!!"));

 
  // create the event group to handle wifi events
  wifi_event_group = xEventGroupCreate();
  
  // initialize the tcp stack
  tcpip_adapter_init();
  
  // initialize the wifi event handler
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  
  // initialize the wifi stack in STAtion mode with config in RAM
  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  
  // configure the wifi connection and start the interface
  wifi_config_t wifi_config = {
    .sta = {
      .ssid = WIFI_SSID,
      .password = WIFI_PASS,
    },
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  NABTO_LOG_INFO(("Connecting to %s\n", WIFI_SSID));
  
  // start the main task
  xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}
