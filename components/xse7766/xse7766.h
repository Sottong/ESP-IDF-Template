#ifndef CSE7766_H
#define CSE7766_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "string.h"

#include <esp_log.h>
#include "esp_system.h"
// #include "esp_log.h"
// #include "driver/gpio.h"
#include "driver/uart.h"

static const char* TAG = "[UART_CSE7766]";

static const float CSE7766_V1R = 1.0; //1mR current Resistor
static const float CSE7766_V2R = 1.0; //1M voltage resistor

typedef struct{

    double vRatio;
    double iRatio;
    double pRatio;

    double _active;
    uint32_t _voltage;
    double _current;
    double _energy;

    uint8_t buffer[24];

    uart_port_t UART_num;
}cse7766_t;


esp_err_t cse7766_read(cse7766_t* cse7766);






#endif