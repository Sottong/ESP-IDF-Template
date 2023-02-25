#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#define RTOS_MS(ms) pdMS_TO_TICKS(ms)
#define RTOS_SEC(sec) pdMS_TO_TICKS(sec*1000)

#define TASK_DELAY_MS(ms) vTaskDelay(RTOS_MS(ms))
#define TASK_DELAY_SEC(sec) vTaskDelay(RTOS_SEC(sec))


//#include "protocol_examples_common.h"

/* modul init */
void wifi_init();
void mqttc_client_init();

/* global flag */

bool flag_wifi_sta_connected;
bool flag_mqtt_connected;

/*  MQTT    */

// Topic 
extern const char* TOPIC_HEADER;

extern const char* TOPIC_DEVICE;

extern const char* TOPIC_SEN_VOL;
extern const char* TOPIC_SEN_CRN;
extern const char* TOPIC_SEN_PWR;
