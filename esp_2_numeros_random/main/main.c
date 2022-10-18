/*LIBRERIAS*/
#include <stdio.h>
#include <stdbool.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "math.h"

/*MACROS*/

#define TAG "DELAY"

/*Declaración de subfunciones*/

int dice_role(void);

/*PRINCIPAL*/
void app_main(void)
{
    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Genera tiempo por ticks
        ESP_LOGI(TAG, "random number %i", dice_role());
    }
}

/*Definición de subfunciones*/
int dice_role(void)
{
    int random = esp_random();
    int positive_number = abs(random);
    int dice_number = (positive_number % 6) + 1;
    return dice_number;
}