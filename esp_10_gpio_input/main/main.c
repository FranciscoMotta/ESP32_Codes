/*
    Inclusiones
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

/*
    Macros
*/

#define LED GPIO_NUM_2
#define BUTTON GPIO_NUM_15

/*
    Principal
*/

void app_main(void)
{
    // Configuramos el LED
    esp_rom_gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);

    // Configuramos el boton
    esp_rom_gpio_pad_select_gpio(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_pulldown_en(BUTTON);

    while (true)
    {
        int state_led = gpio_get_level(BUTTON);
        gpio_set_level(LED, state_led);
        vTaskDelay(1);
    }
}
