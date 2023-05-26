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
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"
#include "dht11.h"
#include "driver/gpio.h"

#define WIFI_SSID "Lâm dâm"
#define WIFI_PASS "204204204"
#define MAX_RETRY 10
static int retry = 0;
#define DHT_GPIO 27

#define WIFI_CONNECTED_BIT BIT1
#define WIFI_FAIL_BIT      BIT0
uint32_t MQTT_CONNECTED = 0;
#define MQTT_PUB_TEMP "esp32/dht/temp"
#define MQTT_PUB_HUM  "esp32/dht/hum"  


// dang ki nhat ky
static const char *TAG = "DEMO_MQTT";
static EventGroupHandle_t s_wifi_event_group;
static esp_mqtt_client_handle_t client = NULL;
// xu ly su kien ket noi toi router
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{   
    if(event_id == WIFI_EVENT_STA_START)
    {
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
    else if(event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        //thu lai toi da 10 lan
        if(retry < MAX_RETRY)
        {
            ESP_ERROR_CHECK(esp_wifi_connect());
            ESP_LOGI(TAG,"connect to AP fail %d", retry + 1);
            ESP_LOGI(TAG,"RETRY");
            retry++;
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data; // ep kieu lay IP  
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        retry = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        mqtt_app_start();
    }
}

static void mqtt_event_handle(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", event_base, event_id);
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        MQTT_CONNECTED = 1;
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        MQTT_CONNECTED = 0;
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "Starting MQTT");
    esp_mqtt_client_config_t mqttConfig = { .uri = ""}; // dia chi IP MQTT broker
    client = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(client,
                                    ESP_EVENT_ANY_ID,
                                    mqtt_event_handler,
                                    client);
    esp_mqtt_client_start(client);
}

void wifi_init_sta(void)
{
    //theo doi su kien wifi
    s_wifi_event_group = xEventGroupCreate();
    //tao tac vu lwip
    ESP_ERROR_CHECK(esp_netif_init());
    //tao vong lap su kien mac dinh
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    //khoi tao che do ket noi toi wifi
    esp_netif_create_default_wifi_sta();
    //cau hinh wifi mac dinh va tao vong lap
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init));
    // Xu ly su kien IP vs ID
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    // Dang ki thanh ghi
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));
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
    // Xu ly su kien bit doi 1 trong 2 bit connect hoac fail duoc dat
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                            pdFALSE, // khong xoa bit duoc doi
                                            pdFALSE, // khong doi tat ca cac bit
                                            portMAX_DELAY );
    if (bits & WIFI_FAIL_BIT) // Dam bao su kien bit da duoc xu ly va co fail bit
    {
        ESP_LOGI(TAG,"Failed to connect to SSID: %s, PASS: %s", WIFI_SSID, WIFI_PASS);
    }
    else if(bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "Connected to AP");
    }
        else
        {
            ESP_LOGI(TAG, "Wi-Fi does not work out");
        }
}

void DHT_Pub_task (void *pvParameter)
{
    gpio_set_direction(DHT_GPIO, GPIO_MODE_INPUT);
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
    xTaskCreate();
}