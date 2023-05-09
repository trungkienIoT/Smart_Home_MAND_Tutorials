#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
//#include "protocol_examples_common.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#define WIFI_SSID "Mandevices Laboratory"
#define WIFI_PASS "MANDevices"
#define MAX_RETRY 10

static int retry = 0;
// dang ki nhat ky
static const char *TAG = "DEMO_MQTT";
// xu ly su kien ket noi toi router
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{   
    if(event_id == WIFI_EVENT_STA_START)
    {
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
    else if(event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        //thu lai toi da 10 lan
        if(retry_id < MAX_RETRY)
        {
            ESP_ERROR_CHECK(esp_wifi_connect());
            ESP_LOGI(TAG,"connect to AP fail");
            ESP_LOGI(TAG,"RETRY");
            retry++;
        }
        else
        {
            ESP_LOGI(TAG, "Can't connect to AP");
        }
    }
    else if(event_id == SYSTEM_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(TAG, "Wi-Fi connected");
    }
    else if (event_id == IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        retry = 0;
        mqtt_app_start();
        ESP_LOGI(TAG, "Starting MQTT");
    }
}

void wifi_init_sta(void)
{
    //tao tac vu lwip
    ESP_ERROR_CHECK(esp_netif_init());
    //tao vong lap su kien mac dinh
    ESP_ERROR_CHECK(esp_event_loop_create_default);
    //khoi tao che do ket noi toi wifi
    esp_netif_create_default_wifi_sta();
    //cau hinh wifi mac dinh va tao vong lap
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init));
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_config_t wifi_config =
    {
        .sta = 
        {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main(void)
{
    //tao phan vung NVS
    esp_err_t nvs = nvs_flash_init();
    if(nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
    ESP_ERROR_CHECK(nvs_flash_erase());
    nvs = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs);
    ESP_LOGI(TAG, "ESP_MODE_STA");
    wifi_init_sta(); //khoi tao ket noi wifi
}