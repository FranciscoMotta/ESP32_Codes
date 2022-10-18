/*LIBRERÍAS*/
#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*MACROS*/

#define LED_BLINK 2
#define TIME_MS_BLINK 1000

/*DECLARACIÓN DE SUBFUNCIONES*/

/*PRINCIPAL*/
void app_main(void)
{
    esp_rom_gpio_pad_select_gpio(LED_BLINK);         // Se selecciona el PIN2
    gpio_set_direction(LED_BLINK, GPIO_MODE_OUTPUT); // Pin 2 como salida
    bool led_on = false;                             // Variable de cambio de nivel
    while (true)
    {
        gpio_set_level(LED_BLINK, led_on); // Se escribe el nivel sobre el pin
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (led_on == 0)
        {
            printf("LED ON\n");
        }
        else
        {
            printf("LED OFF\n");
        }
        led_on = !led_on; // Cambio de nivel
    }
}

/*DEFINICIÓN DE SUBFUNCIONES*/