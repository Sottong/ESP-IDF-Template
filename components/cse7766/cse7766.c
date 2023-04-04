#include "cse7766.h"

/**
* @brief Start HLW8032 UART serial communication according to params
* @param[in] hlw8032 HLW8032 structure
* @param[in] UART_number UART port number to be used
* @param[in] RX_pin_number GPIO number for receiving data
* @param[in] UART_buf_size Buffer size in bytes
*/
esp_err_t cse7766_serial_begin(cse7766_t* cse7766, uart_port_t UART_number, uint16_t UART_buf_size){
    
    esp_err_t err;

    uart_config_t uart_config = {
        .baud_rate = 4800,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        // .source_clk = UART_SCLK_APB,
    };

    err = uart_param_config(UART_number, &uart_config);

    if (err != ESP_OK) {
        ESP_LOGE("HLW8032_TAG", "UART param config failed");
        return err;
    }

    err = uart_driver_install(UART_number, UART_buf_size, 0, 0, NULL, 0);
    if (err != ESP_OK) {
        ESP_LOGE("HLW8032_TAG", "UART driver install failed");

        return err;
    }

    err = uart_enable_swap();

    cse7766->UART_num = UART_number;
    cse7766->iRatio = 1.0;
    cse7766->pRatio = 1.0;
    cse7766->vRatio = 1.0;

    return err;
}


esp_err_t cse7766_read(cse7766_t* cse7766){

    uint8_t check_search = 0;
    size_t data_len = 0;
    uart_get_buffered_data_len(cse7766->UART_num, &data_len);

    while ((int)data_len > 0 && check_search != 0x55) {
        uart_read_bytes(cse7766->UART_num, &check_search, 1, portMAX_DELAY);
        uart_get_buffered_data_len(cse7766->UART_num, &data_len);
    }

    //data pertama harus 0x55 atau 0xF? (check_search == 0x55 || check_search > 0xF0)
    if((int)data_len >=23 && check_search == 0x55){

        uint8_t* data = (uint8_t*)cse7766->buffer;
        // u32_t data [24] = {0x55, 0x5A, 0x02, 0xE9, 0x50, 0x00, 0x03, 0x31, 0x00, 0x3E, 0x9E, 0x00, 0x0D, 0x30, 0x4F, 0x44, 0xF8, 0x00, 0x12, 0x65, 0xF1, 0x81, 0x76, 0x72};

        data[0] = check_search;
        const int rxBytes = uart_read_bytes(cse7766->UART_num, data + 1, 23, portMAX_DELAY);
        // const int rxBytes = 1;

        if(rxBytes > 0){
            
            //data ke dua harus 0x5A
            if(data[1] != 0x5A){
                ESP_LOGW(TAG, "Invalid status REG -> 0x%02X", data[1]);
                uart_flush(cse7766->UART_num);
                return ESP_FAIL;
            }

            //checksum check
            unsigned char checksum = 0;
            for (unsigned char i = 2; i < 23; i++) {
                checksum += data[i];
            }

            if (checksum != data[23]) {
                ESP_LOGW(TAG, "Checksum failed");
                uart_flush(cse7766->UART_num);
                cse7766->buffer[23] = 2;
                return ESP_FAIL;
            }


            //proses parsing data

            //calibration coefficients
            unsigned long _coefV = (data[2]  << 16 | data[3]  << 8 | data[4] );              // 190770
            unsigned long _coefC = (data[8]  << 16 | data[9]  << 8 | data[10]);              // 16030
            unsigned long _coefP = (data[14] << 16 | data[15] << 8 | data[16]); 

            // cse7766->vRatio = 0.99392726735343;
            // cse7766->iRatio = 0.84286745591151;
            // cse7766->pRatio = 0.84197914455076;

            // Adj: this looks like a sampling report
            uint8_t adj = data[20]; 

            //calculate voltage
            cse7766->_voltage = 0;
            if((adj & 0x40) == 0x40){
                unsigned long voltage_cycle = data[5] << 16 | data[6] << 8 | data[7];        // 817
                cse7766->_voltage = cse7766->vRatio * _coefV / voltage_cycle / CSE7766_V2R;          // 190700 / 817 = 233.41 
            }

            // Calculate power
            cse7766->_active = 0;
            if ((adj & 0x10) == 0x10) {
                if ((data[0] & 0xF2) != 0xF2) {
                    unsigned long power_cycle = data[17] << 16 | data[18] << 8 | data[19];   // 4709
                    cse7766->_active = cse7766->pRatio * _coefP / power_cycle / CSE7766_V1R / CSE7766_V2R;       // 5195000 / 4709 = 1103.20
                }
            }

            // Calculate current
            cse7766->_current = 0;
            if ((adj & 0x20) == 0x20) {
                if (cse7766->_active > 0) {
                    unsigned long current_cycle = data[11] << 16 | data[12] << 8 | data[13]; // 3376
                    cse7766->_current = cse7766->iRatio * _coefC / current_cycle / CSE7766_V1R;                  // 16030 / 3376 = 4.75
                }
            }

            // Calculate energy
            unsigned int difference;
            static unsigned int cf_pulses_last = 0;
            unsigned int cf_pulses = data[21] << 8 | data[22];
            if (0 == cf_pulses_last) cf_pulses_last = cf_pulses;
            if (cf_pulses < cf_pulses_last) {
                difference = cf_pulses + (0xFFFF - cf_pulses_last) + 1;
            } else {
                difference = cf_pulses - cf_pulses_last;
            }
            cse7766->_energy += difference * (float) _coefP / 1000000.0;
            cf_pulses_last = cf_pulses;


        }
        else {
        cse7766->buffer[23] = 1;
        return ESP_FAIL;
        }
    }
    else{
        cse7766->buffer[23] = 0;
        return ESP_FAIL;
    }
   


    return ESP_OK;
}
