/* Inclusión de archivos */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "math.h"

/* Enumeraciones */

enum
{
    CORE_1 = 0,
    CORE_2
};

/* Variables */

uint8_t data_UART_fake = 0;

/* Declaración de funciones */

void sender(void *params);
void received(void *params);

/* Principal */

static TaskHandle_t receiverHandler = NULL;

void app_main(void)
{
    xTaskCreatePinnedToCore(&received, "sender", 2048, "Core: 1", 2, &receiverHandler, CORE_1);
    xTaskCreatePinnedToCore(&sender, "receiver", 2048, "Core: 2", 2, NULL, CORE_2);
}

void sender(void *params)
{
    while (true)
    {
        xTaskNotify(receiverHandler,(1 << 0), eSetValueWithoutOverwrite); // Generamos la notificación
        vTaskDelay(pdMS_TO_TICKS(2000));
        xTaskNotify(receiverHandler,(1 << 1), eSetValueWithoutOverwrite); // Generamos la notificación
        vTaskDelay(pdMS_TO_TICKS(2000));
        xTaskNotify(receiverHandler,(1 << 2), eSetValueWithoutOverwrite); // Generamos la notificación
        vTaskDelay(pdMS_TO_TICKS(2000));
        xTaskNotify(receiverHandler,(1 << 3), eSetValueWithoutOverwrite); // Generamos la notificación
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void received(void *params)
{
    uint state;
    while (true)
    {
        xTaskNotifyWait(0,0,&state,portMAX_DELAY); // Esperamos la notificación
        printf("Notificación state en: %d\n", state);
    }  
}
