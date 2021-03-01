#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

void lifecycle_task(void *pvParameter){
    printf("task started\n");
    int timer = 0;
    
    for(;;){
        vTaskDelay(1000/portTICK_RATE_MS);
        timer++;

        if (timer%3 == 0){
            printf("weer 3 seconden\n");
        }

        if (timer%6==0){
            vTaskPrioritySet(NULL,uxTaskPriorityGet(NULL)-1);
            
            printf("task priority set to %d\n",uxTaskPriorityGet(NULL));
            
            if (uxTaskPriorityGet(NULL) == 1){
                printf("Task is being deleted!\n");
                vTaskDelete(NULL);
            }
        }
    }
}

void app_main()
{
    ets_timer_init();
    xTaskCreate(&lifecycle_task, "lifecycle_task", 2048, NULL, 5, NULL);
    for(;;){
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}