#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include <time.h>

#define BUFFERSIZE 10
#define FULL 1
#define EMPTY 0

typedef struct manager_t{
    xSemaphoreHandle bufferSemaphore;
    int status;
    int buffer[BUFFERSIZE];
} Manager;

Manager manager;

void producerTask(void* pvParameters){
    for(;;){    
        do{
            // Wait for Empty status
            while (manager.status != EMPTY)
            {
                vTaskDelay(100/portTICK_PERIOD_MS);
            }
            // Wait for semaphore
        }   while (xSemaphoreTake(manager.bufferSemaphore,100/portTICK_PERIOD_MS)==pdFALSE);
        
        // Produce item
        int data[BUFFERSIZE];
        for (int i = 0; i < BUFFERSIZE; i++)
        {
            data[i] = rand()%100;
        }

        for (int i = 0; i < BUFFERSIZE; i++)
        {
            manager.buffer[i] = data[i];
        }
        printf("%s wrote to buffer in tick %d\n",pcTaskGetTaskName(NULL),xTaskGetTickCount());
        

        // Signal mutex
        manager.status = FULL;

        // Signal full
        xSemaphoreGive(manager.bufferSemaphore);
    }
}

void consumerTask(void* pvParameters){

    for(;;){
        do{
            // Wait for FULL status
            while (manager.status != FULL)
            {
                vTaskDelay(100/portTICK_PERIOD_MS);
            }
            // Wait for semaphore
        }   while (xSemaphoreTake(manager.bufferSemaphore,100/portTICK_PERIOD_MS)==pdFALSE);
        
        // Remove item from buffer
        int data[BUFFERSIZE];

        for (int i = 0; i < BUFFERSIZE; i++)
        {
            data[i] = manager.buffer[i];
        }
        

        // Set status
        manager.status = EMPTY;

        // Give semaphore
        xSemaphoreGive(manager.bufferSemaphore);


        // Consume item
        printf("%s consumed: ",pcTaskGetTaskName(NULL));

        for (int i = 0; i < BUFFERSIZE; i++)
        {
            // Print buffer
            printf("%d ",data[i]);            
        }
        printf("\n");            
        
    }
}

void app_main()
{
    manager.bufferSemaphore = xSemaphoreCreateBinary();
    manager.status = EMPTY;

    xSemaphoreGive(manager.bufferSemaphore);

    xTaskCreate(consumerTask,"Consumer1",2048,NULL,3,NULL);
    xTaskCreate(consumerTask,"Consumer2",2048,NULL,3,NULL);
    xTaskCreate(producerTask,"Producer1",2048,NULL,3,NULL);
    xTaskCreate(producerTask,"Producer2",2048,NULL,3,NULL);
    xTaskCreate(producerTask,"Producer3",2048,NULL,3,NULL);

    ets_timer_init();
	
    for(;;){
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}