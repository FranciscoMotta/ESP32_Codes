/*
    Inclusiones
*/
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <driver/dac.h>

/*
    Macros
*/

#define LED GPIO_NUM_2

/*
    Declaración de funciones
*/

void Gpio_User_Init (void);

/*
    Principal
*/

void app_main(void)
{
    // Configuramos el led
    Gpio_User_Init();
    // Configuramos el adc
    adc1_config_width(ADC_WIDTH_BIT_9);
    adc1_config_channel_atten(ADC_CHANNEL_0, ADC_ATTEN_DB_11);
    // Configuramos el dac
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_voltage(DAC_CHANNEL_1, 0);
    bool state_led = false;
    uint16_t millis_count = 0;
    // Bucle
    while (true)
    {
        /* code */
        int adc_raw_value = adc1_get_raw(ADC_CHANNEL_0); // Leemos el adc
        uint8_t dac_val = adc_raw_value / 2;
        printf("Valor leido: %d\n\r", adc_raw_value);
        dac_output_voltage(DAC_CHANNEL_1, dac_val);
        vTaskDelay(pdMS_TO_TICKS(10));
        millis_count++;
        if (millis_count == 10)
        {
            millis_count = 0;
            state_led = !state_led;
            gpio_set_level(LED, state_led);
        }
        
    }
}

/*
    Definiciones de funciones
*/

void Gpio_User_Init (void)
{
    gpio_config_t led_config;
    led_config.mode = GPIO_MODE_OUTPUT;
    led_config.pull_down_en = 0;
    led_config.pull_up_en = 0;
    led_config.pin_bit_mask = (1ULL << LED);
    gpio_config(&led_config);
}
