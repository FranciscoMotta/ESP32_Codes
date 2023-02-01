/*
    LIBRERIAS
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/touch_sensor.h"
#include "driver/touch_pad.h"

/*
    MACROS
*/

/*
    VARIABLES
*/

/*
    DECLARACIÓN DE FUNCIONES
*/

void User_Init_Touch(void);
void User_Init_Gpio(void);

/*
    PRINCIPAL
*/

void app_main(void)
{
    // Configuramos el touch pad
    User_Init_Touch();
    // Configuramos los Gpio del usuario
    User_Init_Gpio();
    // Bucle
    uint16_t val_touch;
    bool state_led = false;
    while (true)
    {
        state_led = !state_led;
        touch_pad_read(TOUCH_PAD_NUM3, &val_touch);
        printf("Touch sens.: %d\n", val_touch);
        gpio_set_level(GPIO_NUM_2, state_led);
        vTaskDelay(pdMS_TO_TICKS(200));
        if (val_touch < 300)
        {
            gpio_set_level(GPIO_NUM_25, 1);
        }
        else
        {
            gpio_set_level(GPIO_NUM_25, 0);
        }
    }
}

/*
    DEFINICION DE SUBFUNCIONES
*/

void User_Init_Gpio(void)
{
    gpio_config_t led_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = (1ULL << GPIO_NUM_2) | (1ULL << GPIO_NUM_25)};
    gpio_config(&led_conf);
}

void User_Init_Touch(void)
{
    touch_pad_init();                                                              // Iniciamos el touch pad
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V); // Configuramos los niveles de voltaje
    touch_pad_config(TOUCH_PAD_NUM3, -1);                                          // Configuramos el canal del touch pad
}