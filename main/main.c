#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_timer.h"

int count = 0;
void timer_callback(void* arg)
{
    printf("timmer function ke : %d\n", count);// kode yang dijalankan setiap timer berakhir
    count++;
}

void setup_timer()
{
    esp_timer_handle_t timer_handle; // deklarasi handle timer
    esp_timer_create_args_t timer_args = {
        .callback = &timer_callback, // fungsi callback ketika timer berakhir
        .name = "my_timer", // nama timer (opsional)
        .arg = NULL // argumen tambahan untuk fungsi callback (opsional)
    };
    esp_timer_create(&timer_args, &timer_handle); // membuat timer dengan opsi yang dikonfigurasi
    esp_timer_start_periodic(timer_handle, 1000000); // memulai timer dengan jeda 1 detik
}

void app_main()
{
    setup_timer();
    // kode lainnya
}