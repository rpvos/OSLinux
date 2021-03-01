#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"


void app_main()
{
    ets_timer_init();
	
    for(;;){
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}