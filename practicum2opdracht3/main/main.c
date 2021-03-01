#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

void name_task(void *pvParameters){
    for(;;){
        printf("%s\n",pcTaskGetTaskName(NULL));
    }
}


void app_main()
{
    ets_timer_init();


    for(int i = 1; i<=20;i++){
        char name[] = "task20";
        name[4] = i/10+'0';
        name[5] = i%10+'0';
        if (i<10)
        {
        xTaskCreate(&name_task, name, 2048,NULL,5,NULL );
        }
        else{
        xTaskCreate(&name_task, name, 2048,NULL,3,NULL );
        }        
    }
    
	
    //vTaskStartScheduler();

    for(;;){
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}