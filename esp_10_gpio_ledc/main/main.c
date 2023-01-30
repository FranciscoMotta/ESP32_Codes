/*
    Inclusiones
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/ledc.h"

/*
    Funciones
*/

void User_Ledc_Init (void);
void User_Adc_Init (void);
void User_Gpio_Init (void);

/*
    Principal
*/


void app_main(void)
{
    // Iniciamos el LedC
    User_Ledc_Init();

    // Iniciamos el ADC (Trataremos de leer el ADC del hall XD)
    User_Adc_Init();

    // Iniciamos el led del usuario
    User_Gpio_Init();

    // Configuramos
    ledc_fade_func_install(0);
    // Hacemos un bucle para ver el funcionamiento
    for(uint16_t i = 0 ; i < 1024 ; i++)
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Contador
    uint32_t contador = 0;
    bool state_led = false;

    ledc_fade_func_install(0); // Por si las moscas
    while (true)
    {
        contador++;
        state_led = !state_led;
        if (contador > 1000)
        {
            contador = 0;
        }
        else 
        {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, contador);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        }
        if (contador % 10)
        {
            int hall_read = adc1_get_raw(ADC_CHANNEL_0);
            printf("Hall sens: %i\n", hall_read);
        }

        if (contador % 20)
        {
            gpio_set_level(GPIO_NUM_25, state_led);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }   
}

/*
    Definiciones de funciones
*/

void User_Gpio_Init (void)
{
    gpio_config_t led_blink_config = 
    {
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .pin_bit_mask = (1ULL << GPIO_NUM_25)
    };

    gpio_config(&led_blink_config);

    gpio_set_level(GPIO_NUM_2, 0);
}

void User_Ledc_Init (void)
{
        // Configuramos el timer del ledc
    ledc_timer_config_t timer = 
    {
        .speed_mode = LEDC_LOW_SPEED_MODE, // Velocidad
        .duty_resolution = LEDC_TIMER_10_BIT, // Resolución del duty cicle
        .timer_num = LEDC_TIMER_0, // Timer a usar
        .freq_hz = 5000, // Frecuencia
        .clk_cfg = LEDC_AUTO_CLK // Selección del reloj
    };
    ledc_timer_config(&timer);
    
    // Configuramos el canal del ledc
    ledc_channel_config_t channel = 
    {
        .gpio_num = GPIO_NUM_2, // Asignamos un GPIO
        .speed_mode = LEDC_LOW_SPEED_MODE, // Elegimos la velocidad (misma que el anterior)
        .channel = LEDC_CHANNEL_0, // Elegimos el canal 
        .timer_sel = LEDC_TIMER_0, // Seleccionamos el timer (mismo que el anterior)
        .duty = 1, // Iniciamos el duty de [1 - 2^resolucion]
        .hpoint = 0 // threshold de ajuste fino
    };
    ledc_channel_config(&channel);
}

void User_Adc_Init(void)
{
    adc1_config_width(ADC_WIDTH_BIT_9); // Resolucion del adc
    adc1_config_channel_atten(ADC_CHANNEL_0, ADC_ATTEN_DB_11); // Atenuación del adc
}