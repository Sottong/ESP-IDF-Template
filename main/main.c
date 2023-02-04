/* Hello World Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <ds1307.h>



void app_main()
{
  int i=0;
  while(1) {
        printf("Hello world! [%d] \n", i);
      i++;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

}