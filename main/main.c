#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <esp_system.h>
#include <esp_event.h>

#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_sntp.h>
#include <esp_timer.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <esp_err.h>

#include <cJSON.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <lwip/apps/sntp.h>

#include <driver/gpio.h>
#include <driver/uart.h>

//local Lib
#include "cse7766.h"
#include "xse7766.h"

#define RTOS_MS(ms) pdMS_TO_TICKS(ms)
#define RTOS_SEC(sec) pdMS_TO_TICKS(sec*1000)

#define TASK_DELAY_MS(ms) vTaskDelay(RTOS_MS(ms))
#define TASK_DELAY_SEC(sec) vTaskDelay(RTOS_SEC(sec))

static const size_t TSTACK_CSE7766 = 3*1024;
static const UBaseType_t TPRIO_CSE7766 = 5;

static const u16_t CSE7766_READ_SEC = 5;

cse7766_t cse7766;

float tes = 1.2;

void cse7766_setRatio(cse7766_t* cse7766){

    cse7766->vRatio = 0.99392726735343;
    cse7766->iRatio = 0.84286745591151;
    cse7766->pRatio = 0.84197914455076;
    printf("R2 %f\n",cse7766->vRatio);
}

static void rT_cse7766_rcv(void* arg){

    esp_err_t err;
    size_t data_len;
    u32_t energy_buf;

    while (1){


        err = cse7766_read(&cse7766);


        ESP_LOGE(TAG, "nilai teganagan : %.3f", cse7766._voltage);


       TASK_DELAY_SEC(CSE7766_READ_SEC);
    }
}



void app_main()
{
    printf("tes double %f \n", tes);
    ESP_LOGI(TAG, "print float = %e", tes);

    // esp_err_t err;

    // err = cse7766_serial_begin(&cse7766, UART_NUM_0, 256);
    // assert(err == ESP_OK && "cse7766 uart init failed");

    cse7766_setRatio(&cse7766);
    printf("R3 %f\n",cse7766.vRatio);

    xTaskCreate(rT_cse7766_rcv, "cse7766_rcv", TSTACK_CSE7766, NULL, TPRIO_CSE7766, NULL);
}