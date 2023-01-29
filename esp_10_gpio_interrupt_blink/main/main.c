/*
    Incluiones
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
    Rutina de servicio a interrupciones
*/

uint8_t counter = 0;

static void IRAM_ATTR interrupt_gpio(void *args)
{
    int state_push = gpio_get_level((int) args);
    if (state_push)
    {
        counter++;
        if (counter == 5)
        {
            counter = 0;
        }
    }
}

/*
    Declaracion de funciones
*/

void Gpio_User_Config (void);

/*
    Variables
*/

uint16_t periods[5] = {100, 200, 500, 750, 1000};

/*
    Principal
*/

void app_main(void)
{
    // Configuramos el GPIO
    Gpio_User_Config();

    // Configuramos las interrupciones
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON, interrupt_gpio, (void *)BUTTON);

    // Bucle
    bool state = false;
    while (true)
    {
        /* code */
        state = !state;
        gpio_set_level(LED, state);
        vTaskDelay(pdMS_TO_TICKS(periods[counter]));
    }
}

/*
    Definicion de funciones
*/

void Gpio_User_Config (void)
{
    // Configuramos el LED
    gpio_config_t config;
    config.mode = GPIO_MODE_OUTPUT;
    config.pull_down_en = 0;
    config.pull_up_en = 0;
    config.pin_bit_mask = (1ULL << LED);
    gpio_config(&config);

    // Configuramos el boton
    gpio_config_t button_config;
    button_config.intr_type = GPIO_INTR_POSEDGE;
    button_config.pull_down_en = 1;
    button_config.pull_up_en = 0;
    button_config.pin_bit_mask = (1ULL << BUTTON);
    gpio_config(&button_config);
}