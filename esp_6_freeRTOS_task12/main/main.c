/* Inclusión de archivos */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Declaración de funciones */

void task1 (void * params);
void task2 (void * params);

/* Enumeraciones */

enum
{
    FIRST_CORE = 0,
    SECOND_CORE
};

enum
{
    FIRST_PRIORITY = 1,
    SECOND_PRIORITY,
    THIRT_PRIORITY,
    FOURTH_PRIORITY,
    FIFTH_PRIORITY
};

/* Principal */
void app_main(void)
{
    //        dir. func      task name         mem     param  prior idk
    xTaskCreate(&task1, "temperature reading", 2048, "task 1", 2, NULL);
    //        dir. func      task name         mem     param  prior idk
    xTaskCreatePinnedToCore(&task2, "temperature reading", 2048, "task 2", 2, NULL, FIRST_CORE);
    /* Prioridad puede ser entre 1 a 5 */
}

/* Definición de funciones */

void task1 (void * params)
{
    while (true)
    {
        printf("Midiendo temperadura en: %s\n", (char *)params);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task2 (void * params)
{
    while (true)
    {
        printf("Midiendo temperatura en: %s\n", (char *)params);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}