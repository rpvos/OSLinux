#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

#define BLINK_GPIO 2
#define ROOD_GPIO 12
#define GROEN_GPIO 13

void rood_task(void *pvParameters){
    double hertz = 1;
    gpio_pad_select_gpio(ROOD_GPIO);
    gpio_set_direction(ROOD_GPIO, GPIO_MODE_OUTPUT);

    for(;;){
        vTaskDelay((1000/hertz)/portTICK_PERIOD_MS);
        gpio_set_level(ROOD_GPIO,1);
        vTaskDelay((1000/hertz)/portTICK_PERIOD_MS);
        gpio_set_level(ROOD_GPIO,0);
    }
}

void groen_task(void *pvParameters){
    double hertz = 0.2;
    gpio_pad_select_gpio(GROEN_GPIO);
    gpio_set_direction(GROEN_GPIO, GPIO_MODE_OUTPUT);

    for(;;){
        vTaskDelay((1000/hertz)/portTICK_PERIOD_MS);
        gpio_set_level(GROEN_GPIO,1);
        vTaskDelay((1000/hertz)/portTICK_PERIOD_MS);
        gpio_set_level(GROEN_GPIO,0);
    }
}

void app_main()
{
    ets_timer_init();
	
    xTaskCreate( &rood_task, "Rood", 1000, NULL, 1, NULL );
    xTaskCreate( &groen_task, "Groen", 1000, NULL, 1, NULL );

    for(;;){
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}