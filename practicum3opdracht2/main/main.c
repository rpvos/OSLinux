#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "semaphore.h"

sem_t writerSemaphore,readerSemaphore;


void writerTask(void *pvParameters){
    
    for(;;){

    }
}

void readerTask(void *pvParameters){

    for(;;){
        //xSemaphoreTake(readerSemaphore,2000/portTICK_PERIOD_MS);
        printf("Taking semaphore");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        //xSemaphoreGive(readerSemaphore);
        printf("Giving semaphore");
    }
}

void app_main()
{
    xSemaphoreCreateBinary();

    xTaskCreate(readerTask,"Reader",2048,NULL,3,NULL);
    xTaskCreate(readerTask,"Reader",2048,NULL,3,NULL);

    ets_timer_init();
	
    for(;;){
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}