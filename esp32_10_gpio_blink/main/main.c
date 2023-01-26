/*
    Inclusiones
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*
    Macros
*/

#define LED_PIN     2

typedef enum{
    LED_OFF = 0,
    LED_ON
}_led_state_t;

/*
    Declaración de funciones
*/


/*
    Principal
*/
void app_main(void)
{
    esp_rom_gpio_pad_select_gpio(LED_PIN); // Seleccionamos el pin
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    _led_state_t state_led = LED_OFF;

    while (true)
    {
        state_led = !state_led;
        switch (state_led)
        {
        case LED_OFF:
        gpio_set_level(LED_PIN, LED_OFF);
        printf("LED PRENDIDO\n\r");
            break;
        case LED_ON:
        gpio_set_level(LED_PIN, LED_ON);
        printf("LED APAGADO\n\r");
        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/*
    Definición de funciones
*/