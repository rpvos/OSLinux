#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

// Definitoions for the status of the philosophers
#define EATING 1
#define THINKING 0

// Definitions for all the pins used by the leds
#define ENABLELEDS 1
#define GPIO1 13
#define GPIO2 12
#define GPIO3 14
#define GPIO4 27
#define GPIO5 26

typedef struct philosopher_t{
    xSemaphoreHandle fork;
    int status;
} Philosopher;

Philosopher philosophers[5];

int initializePhilosophers(){
    for (int i = 0; i < 5; i++)
    {
        Philosopher p = {xSemaphoreCreateBinary(), THINKING};
        xSemaphoreGive(p.fork);
        philosophers[i] = p;
    }
    return ESP_OK;
}


// This function initializes all leds
int initializeLeds(){
    gpio_pad_select_gpio(GPIO1);
    gpio_pad_select_gpio(GPIO2);
    gpio_pad_select_gpio(GPIO3);
    gpio_pad_select_gpio(GPIO4);
    gpio_pad_select_gpio(GPIO5);

    gpio_set_direction(GPIO1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO2, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO3, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO4, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO5, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

// This function handles status changes
void setStatus(int philosopher, int status){
    philosophers[philosopher].status = status;

if(ENABLELEDS)
    switch (philosopher)
    {
    case 0:
        gpio_set_level(GPIO1,status);
        break;
    case 1:
        gpio_set_level(GPIO2,status);
        break;
    case 2:
        gpio_set_level(GPIO3,status);
        break;
    case 3:
        gpio_set_level(GPIO4,status);
        break;
    case 4:
        gpio_set_level(GPIO5,status);
        break;
    default:
        break;
    }
}

// Function to check if he can eat
int askForFork(int philosopher){
    // He has a fork himself
    int hasFork = xSemaphoreTake(philosophers[philosopher].fork, 1);

    if(hasFork==pdFALSE){
        setStatus(philosopher,THINKING);
        return THINKING;
    }

    // Check if the person after him has a fork
    if (philosopher == 4)
    {
        hasFork = xSemaphoreTake(philosophers[0].fork, 1);
       if(hasFork){
           setStatus(philosopher,EATING);
           return EATING;
       }
    }else{
        hasFork = xSemaphoreTake(philosophers[philosopher+1].fork, 1);
        if(hasFork){
           setStatus(philosopher,EATING);
           return EATING;
       }
    }

    // Give back the fork to the philosopher because he cant eat
    xSemaphoreGive(philosophers[philosopher].fork);
    setStatus(philosopher,THINKING);
    return THINKING;
}

// Function that returns the forks he took
void returnForks(int philosopher){
    // Check so we dont go out of bounds
    if (philosopher == 4){
        xSemaphoreGive(philosophers[philosopher].fork);
        xSemaphoreGive(philosophers[0].fork);
    }else{
        xSemaphoreGive(philosophers[philosopher].fork);
        xSemaphoreGive(philosophers[philosopher+1].fork);
    }
}

void philosopher_task(void *pvParameters){    
    // This integer keeps track which philosopher this is. 
    // This is needed because this philosopher can only take the fork from a philosopher next to him.
    int philosopher = atoi(pcTaskGetTaskName(NULL));

for(;;){
    // Philosopher asks if he can borrow the fork from the philosophers after him
    askForFork(philosopher);

    // Check the status of the philosopher
    if  (philosophers[philosopher].status == THINKING){
        vTaskPrioritySet(NULL,uxTaskPriorityGet(NULL)+1);
        // He is thinking for 1 second waiting for forks
        vTaskDelay(2000/portTICK_PERIOD_MS);

     } else{
        // Then he gets a lower priority so he wont get the forks before the other philosophers who haven't just eaten
        vTaskDelay(2000/portTICK_PERIOD_MS);
        vTaskPrioritySet(NULL,configMAX_PRIORITIES-1);
        returnForks(philosopher);
        vTaskPrioritySet(NULL,2);

     }
}

    
}


void app_main()
{
    initializeLeds();
    initializePhilosophers();

    ets_timer_init();

// Taskhandlers for all the philosophers
TaskHandle_t handler[5];

// Initiate all the philosophers
for(int i = 0; i<5 ; i++){
    char name[2];
    itoa(i,name,10);
    xTaskCreate(philosopher_task,name,2048,NULL,3,&handler[i]);
}

    // Wait for 100 ms to make sure they have updated their status to THINKING or EATING
    vTaskDelay(100/portTICK_PERIOD_MS);
	
    // For loop that displays the status of every philosopher every 2 seconds
    for(;;){
        vTaskDelay(2000/portTICK_PERIOD_MS);
        for (int i = 0; i < 5; i++)
        {
            if (philosophers[i].status==THINKING){
                printf("Philosopher %s is thinking\n",pcTaskGetTaskName(handler[i]));
            } else{
                printf("Philosopher %s is eating\n",pcTaskGetTaskName(handler[i]));
            }
        }

        printf("--------------------------\n");
    }
}

