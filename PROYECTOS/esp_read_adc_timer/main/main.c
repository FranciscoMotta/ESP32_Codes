/*
   LIBRERIAS
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_system.h"
#include "esp_timer.h"

/*
    MACROS
*/

#define CONV_MS_TO_US(ms)   (uint64_t)(ms)*1000  // Convertimos milisegundos a microsegundos

/*
    VARIABLES
*/

volatile uint32_t counter_timer_ms = 0;
volatile int read_adc_timer = 0;
/*
    DECLARACIÓN DE FUNCIONES
*/

void User_Init_Gpio (void);  // Configuraciones del gpio
void User_Init_Ledc (void);  // Configuramos el LEDC (LED Control)
void User_Init_Adc (void);   // Configuramos del ADC
void User_Init_Timer (void); // Configuramos el Timer

/*
    RUTINA DE SERVICIO A INT. TIMERS
*/

void timer_check(TimerHandle_t xTimer);

/*PRINCIPAL*/
void app_main(void)
{
    // Iniciamos el led del usuario
    User_Init_Gpio();

    // Iniciamos el ledc
    User_Init_Ledc();

    // Iniciamos las configuraciones del adc
    User_Init_Adc();

    // Iniciamos el Timer
    User_Init_Timer();

    // Dejaremos parpadeando del led
    bool state_led = false;
    while (true)
    {
        /* code */
        state_led = !state_led;
        gpio_set_level(GPIO_NUM_25, state_led);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/*DEFINICION DE SUBFUNCIONES*/

void timer_check(TimerHandle_t xTimer)
{
    counter_timer_ms++;
    if (counter_timer_ms == 1024)
    {
        /* code */
        counter_timer_ms = 0;
    }
    else
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, counter_timer_ms);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }
    if (((counter_timer_ms % 500) == 0) && (counter_timer_ms != 0))
    {
        printf("ADC: %d at %lld ms\n\r", adc1_get_raw(ADC_CHANNEL_0), esp_timer_get_time() / 1000);
    }
    
}

void User_Init_Timer(void)
{
    const esp_timer_create_args_t esp_timer = {
        .callback = timer_check,
        .name = "My timer"
    };

    esp_timer_handle_t timer_esp;
    esp_timer_create(&esp_timer, &timer_esp);
    esp_timer_start_periodic(timer_esp, 1000);
}

void User_Init_Adc(void)
{
    adc1_config_width(ADC_WIDTH_BIT_10);                        // Adc al 10bits de resolución
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // Elegimos el canal 0 y atenuacion para 3.3v
}

void User_Init_Ledc(void)
{
    ledc_timer_config_t timer_ledc = {
        .speed_mode = LEDC_LOW_SPEED_MODE,    // Baja velocidad
        .timer_num = LEDC_TIMER_0,            // Elegimos timer 0
        .duty_resolution = LEDC_TIMER_10_BIT, // Resolución a 10bits
        .freq_hz = 5000,                      // Frecuencia de funcionamiento a 5khz
        .clk_cfg = LEDC_AUTO_CLK              // Reloj automático
    };
    ledc_timer_config(&timer_ledc);

    ledc_channel_config_t channel_ledc = {
        .channel = LEDC_CHANNEL_0,         // Elegimos el canal 0
        .gpio_num = GPIO_NUM_2,            // Elegimos el gpio 2 (LED)
        .speed_mode = LEDC_LOW_SPEED_MODE, // Baja velocidad
        .timer_sel = LEDC_TIMER_0,         // Seleccion de timer
        .duty = 0,                         // Ancho de pulso
        .hpoint = 0,                       // Ajuste fino
    };
    ledc_channel_config(&channel_ledc);
}

void User_Init_Gpio(void)
{
    gpio_config_t led_user = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = (1ULL << GPIO_NUM_25)};
    gpio_config(&led_user);
}