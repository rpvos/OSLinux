#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/semphr.h"
#include <assert.h>

#define ENABLED 1
#define DISABLED 0
#define PRESENT 1
#define ABSENT 0

typedef struct manager_t{
    SemaphoreHandle_t writerSemaphore;
    int readerSemaphore;
    bool lock;
    // For writers to go after the curent writer
    int writerQueue;
} Manager;

Manager manager;

void takeWriterSemaphore(){
    // Join the writer queue
    manager.writerQueue++;

    // Check if someone is using writing
    while(xSemaphoreTake(manager.writerSemaphore,1000/portTICK_PERIOD_MS)==pdFALSE){
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
    // Lock that no new readers can get in
    manager.lock = ENABLED;

    // Wait for all the already connected readers to get out
    while (manager.readerSemaphore!=0)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
    // Leave the writer queue
    manager.writerQueue--;
}

void giveWriterSemaphore(){
    assert(xSemaphoreGive(manager.writerSemaphore));
    // If there are writers queued up we dont disable the lock
    if (manager.writerQueue==0)
    {
        manager.lock = DISABLED;
    }
    
}

void takeReaderSemaphore(){
    // If lock is enabled we wait
    while (manager.lock == ENABLED)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
    manager.readerSemaphore++;
}

void giveReaderSemaphore(){
    manager.readerSemaphore--;
}


void writerTask(void *pvParameters){
    
    for(;;){
        takeWriterSemaphore();

// Writing data for 1 second
        printf("%s is writing string\n",pcTaskGetTaskName(NULL));
        vTaskDelay(1000/portTICK_PERIOD_MS);

        giveWriterSemaphore();

// Letting other people write and read
        vTaskDelay(4000/portTICK_PERIOD_MS);

    }
}

void readerTask(void *pvParameters){

    for(;;){
        takeReaderSemaphore();
        
        // Reading for 1 second
        printf("%s is reading string\n",pcTaskGetTaskName(NULL));
        vTaskDelay(100/portTICK_PERIOD_MS);
        
        giveReaderSemaphore();

        vTaskDelay(4000/portTICK_PERIOD_MS);
    }
}

void app_main()
{
    manager.writerSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(manager.writerSemaphore);

    xTaskCreate(writerTask,"Writer1",2048,NULL,3,NULL);
    xTaskCreate(writerTask,"Writer2",2048,NULL,3,NULL);
    xTaskCreate(writerTask,"Writer3",2048,NULL,3,NULL);
    xTaskCreate(readerTask,"Reader1",2048,NULL,3,NULL);
    xTaskCreate(readerTask,"Reader2",2048,NULL,3,NULL);
    xTaskCreate(readerTask,"Reader3",2048,NULL,3,NULL);

    ets_timer_init();
	
    for(;;){
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}