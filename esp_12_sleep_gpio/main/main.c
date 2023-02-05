/*
   LIBRERIAS
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp32/rom/uart.h"
#include "esp_timer.h"
#include "driver/rtc_io.h"

/*
   MACROS
*/

#define USER_WAKE_UP_BUTTON GPIO_NUM_15
#define USER_RELEASE_LED GPIO_NUM_2

/*
   VARIABLES
*/

/*
   DECLARACIÓN DE FUNCIONES
*/

void User_Gpio_Init(void);

/*
   PRINCIPAL
*/

void app_main(void)
{
   // Incluímos las funciones
   User_Gpio_Init();
   // esp_sleep_enable_timer_wakeup(5000000); // Asignamos el sleep a 5seg
   while (true)
   {
      if (rtc_gpio_get_level(USER_WAKE_UP_BUTTON))
      {
         /* code */
         printf("uC is already wake-up, release button\n");
         do
         {
            /* code */
            vTaskDelay(pdMS_TO_TICKS(10));
            gpio_set_level(USER_RELEASE_LED, 1);
         } while (rtc_gpio_get_level(USER_WAKE_UP_BUTTON));
      }

      printf("ESP entrando a modo sleep\n");
      uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM); // Esperamos a terminar el proceso
      gpio_set_level(USER_RELEASE_LED, 0);
      esp_light_sleep_start(); // Iniciamos el modo slepp
      esp_sleep_wakeup_cause_t causa = esp_sleep_get_wakeup_cause();
      printf("ESP salio del modo sleep por causa: %s\n", causa == ESP_SLEEP_WAKEUP_GPIO ? "boton" : "desc."); // Mostramos el tiempo que demoró el modo sleep
   }
}

/*
   DEFINICION DE SUBFUNCIONES
*/

void User_Gpio_Init(void)
{
   // Boton
   gpio_config_t button_config = {
      .intr_type = GPIO_INTR_DISABLE,
       .mode = GPIO_MODE_INPUT,
       .pull_down_en = GPIO_PULLDOWN_ENABLE,
       .pull_up_en = GPIO_PULLUP_DISABLE,
       .pin_bit_mask = (1ULL << USER_WAKE_UP_BUTTON)};
   gpio_config(&button_config);

   // Asignamos el wake-up al gpio del boton
   esp_sleep_enable_gpio_wakeup();
   gpio_wakeup_enable(USER_WAKE_UP_BUTTON, GPIO_INTR_HIGH_LEVEL);

   // Led
   gpio_config_t led_config = {
       .intr_type = GPIO_INTR_DISABLE,
       .mode = GPIO_MODE_DEF_OUTPUT,
       .pull_down_en = GPIO_PULLDOWN_DISABLE,
       .pull_up_en = GPIO_PULLUP_DISABLE,
       .pin_bit_mask = (1ULL << USER_RELEASE_LED)};

   gpio_config(&led_config);
}