#include "main_header.h"

static const char *TAG = "WI-FI";

// #define EXAMPLE_ESP_WIFI_SSID      "ElisaCom_Kos2"
// #define EXAMPLE_ESP_WIFI_PASS      "kosElisa12345"

#define EXAMPLE_ESP_WIFI_SSID      "ElisaCom_Kos"
#define EXAMPLE_ESP_WIFI_PASS      "kosElisa12345"

// #define EXAMPLE_ESP_WIFI_SSID      "123"
// #define EXAMPLE_ESP_WIFI_PASS      "innovate"

#define EXAMPLE_ESP_MAXIMUM_RETRY  5

static const u32_t WIFI_STA_RETRY_SEC = 3;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        
        esp_wifi_connect();
    } 
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED){
        flag_wifi_sta_connected = 1;
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        flag_wifi_sta_connected = 0;
        TASK_DELAY_SEC(WIFI_STA_RETRY_SEC);
        esp_wifi_connect();
        ESP_LOGI(TAG, "Retrying to connect to the AP");
    } 
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
    }
}

void wifi_init_sta(void)
{
    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS 
        },
    };

    /* Setting a password implies station will connect to all security modes including WEP/WPA.
        * However these modes are deprecated and not advisable to be used. Incase your Access point
        * doesn't support WPA2, these mode can be enabled by commenting below line */

    if (strlen((char *)wifi_config.sta.password)) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

void wifi_init()
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}
