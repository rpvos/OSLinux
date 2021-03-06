#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

#define EATING 1
#define THINKING 0
int semaphores[5];
int status[5];

int askForFork(int philosopher){
    // He has a fork himself
    if(semaphores[philosopher]==0){
        return 0;
    }

    // Check if the person after him has a fork
    if (philosopher == 4)
    {
       if( semaphores[0] == 1){
           
           // We know he can take two forks so we take the two forks
           semaphores[0] = 0;
           semaphores[4] = 0;
           return 1;
       }
    }else{
        if( semaphores[philosopher+1] == 1){

           // We know he can take two forks so we take the two forks
           semaphores[philosopher] = 0;
           semaphores[philosopher+1] = 0;
           return 1;
       }
    }

    return 0;
}

void returnForks(int philosopher){
    if (philosopher == 4){
        semaphores[0] = 1;
        semaphores[4] = 1;
    }else{
        semaphores[philosopher] = 1;
        semaphores[philosopher+1] = 1;
    }
}

void philosopher_task(void *pvParameters){    
    // This integer keeps track which philosopher this is. 
    // This is needed because this philosopher can only take the fork from a philosopher next to him.
    int philosopher = atoi(pcTaskGetTaskName(NULL));

    // Philosopher asks if he can borrow the fork from the two philosophers next to him
for(;;){
    status[philosopher] = askForFork(philosopher);

    if  (status[philosopher] == THINKING){
        vTaskPrioritySet(NULL,uxTaskPriorityGet(NULL)+1);
        // He is thinking for 1 second waiting for forks
        vTaskDelay(2000/portTICK_PERIOD_MS);

     } else{
      
        // He eats for two seconds
        vTaskPrioritySet(NULL,configMAX_PRIORITIES-1);
        vTaskDelay(2000/portTICK_PERIOD_MS);
        returnForks(philosopher);
        // Then he thinks for 1 second
        vTaskPrioritySet(NULL,2);
     }
}

    
}


void app_main()
{
    ets_timer_init();

for (int i = 0; i < 5; i++)
{
    semaphores[i] = 1;
}

TaskHandle_t handler[5];

for(int i = 0; i<5 ; i++){
    char name[2];
    itoa(i,name,10);
    xTaskCreate(philosopher_task,name,2048,NULL,3,&handler[i]);
}

    vTaskDelay(100/portTICK_PERIOD_MS);
	
    for(;;){
        vTaskDelay(2000/portTICK_PERIOD_MS);
        for (int i = 0; i < 5; i++)
        {
            if (status[i]==THINKING){
                printf("Philosopher %s is thinking\n",pcTaskGetTaskName(handler[i]));
            } else{
                printf("Philosopher %s is eating\n",pcTaskGetTaskName(handler[i]));
            }
        }

        printf("--------------------------\n");
        
    }
}

