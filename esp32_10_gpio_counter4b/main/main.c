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

#define LED_1           13
#define LED_2           12
#define LED_3           14
#define LED_4           27

#define COUNTER_SIZE    4   

/*
    Variables
*/

uint8_t gpio_pines[] = {LED_1, LED_2, LED_3, LED_4};
uint8_t gpio_masks[] = {0x01, 0x02, 0x04, 0x08};
/*
    Declaración de funciones
*/

void Init_Gpio_Leds (void);
void Send_Count_Leds (uint8_t counter);
/*
    Principal
*/

void app_main(void)
{
    Init_Gpio_Leds(); // Iniciamos los pines
    while (true)
    {
        uint8_t counter;
        for(counter = 0 ; counter < 16 ; counter ++)
        {
            Send_Count_Leds(counter);
            printf("Cuenta: %i\n\r", counter);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }    
}

/*
    Definición de funciones
*/

void Init_Gpio_Leds (void)
{
    uint8_t counter_local;
    for (counter_local = 0 ; counter_local < COUNTER_SIZE; counter_local ++)
    {
        esp_rom_gpio_pad_select_gpio(gpio_pines[counter_local]);
        gpio_set_direction(gpio_pines[counter_local], GPIO_MODE_OUTPUT);
        gpio_set_level(gpio_pines[counter_local], 0);
    }
}

void Send_Count_Leds (uint8_t counter)
{
    uint8_t local_counter;
    for (local_counter = 0; local_counter < COUNTER_SIZE ; local_counter ++)
    {
        if (counter & gpio_masks[local_counter])
        {
            gpio_set_level(gpio_pines[local_counter], 1);
        }
        else
        {
            gpio_set_level(gpio_pines[local_counter], 0);
        }
    }
}