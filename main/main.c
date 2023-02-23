#include "main_header.h"

void app_main(){
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    mqtt_server_init();
}