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
#include "driver/gpio.h"
#include "driver/rtc_io.h"

/*
   MACROS
*/

#define USER_BUTTON_GO_TO_SLEEP GPIO_NUM_0
#define USER_BUTTON_WAKE_UP GPIO_NUM_15
#define USER_LED_NORMAL_FUNC GPIO_NUM_2

/*
   VARIABLES
*/

bool led_state = false;

/*
   DECLARACIÓN DE FUNCIONES
*/

void User_Gpio_Init(void);
void Led_Toggle_Prev_Sleep(void);

/*
   PRINCIPAL
*/

RTC_DATA_ATTR esp_sleep_wakeup_cause_t causa;

void app_main(void)
{
   rtc_gpio_deinit(USER_BUTTON_WAKE_UP);
   User_Gpio_Init();
   bool state_led = false;
   uint16_t count_local = 0;
   esp_sleep_wakeup_cause_t causa = esp_sleep_get_wakeup_cause();
   printf("Esp32 wake-up for: %s\n", causa == ESP_SLEEP_WAKEUP_GPIO ? "gpio" : "desc");
   while (true)
   {
      /* code */
      if (!rtc_gpio_get_level(USER_BUTTON_GO_TO_SLEEP))
      {
         gpio_set_level(USER_LED_NORMAL_FUNC, 1);
         while (!rtc_gpio_get_level(USER_BUTTON_GO_TO_SLEEP))
            ;
         Led_Toggle_Prev_Sleep();
         printf("Esp32 going to sleep\n");
         uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
         esp_deep_sleep_start();
      }
      if (count_local == 50)
      {
         /* code */
         count_local = 0;
         state_led = !state_led;
         gpio_set_level(USER_LED_NORMAL_FUNC, state_led);
      }
      count_local++;
      vTaskDelay(pdMS_TO_TICKS(10));
   }
}

/*
   DEFINICION DE SUBFUNCIONES
*/

void Led_Toggle_Prev_Sleep(void)
{
   uint8_t local_count = 0;
   const uint8_t times = 10;
   for (local_count = 0; local_count < times; local_count++)
   {
      /* code */
      led_state = !led_state;
      gpio_set_level(USER_LED_NORMAL_FUNC, led_state);
      vTaskDelay(pdMS_TO_TICKS(50));
   }
   gpio_set_level(USER_LED_NORMAL_FUNC, 0);
}

void User_Gpio_Init(void)
{
   // Configuramos los botones

   gpio_config_t button1_config = {
       .mode = GPIO_MODE_INPUT,
       .pull_up_en = GPIO_PULLUP_ENABLE,
       .pull_down_en = GPIO_PULLDOWN_DISABLE,
       .pin_bit_mask = (1ULL << USER_BUTTON_GO_TO_SLEEP)};
   gpio_config(&button1_config);

   // Boton de Wake-Up

   rtc_gpio_pullup_en(USER_BUTTON_WAKE_UP);
   rtc_gpio_pulldown_dis(USER_BUTTON_WAKE_UP);
   // Habilitamos el wake up por gpio
   esp_sleep_enable_ext0_wakeup(USER_BUTTON_WAKE_UP, 0);

   // Configuramos el led

   gpio_config_t leds_config = {
       .intr_type = GPIO_INTR_DISABLE,
       .mode = GPIO_MODE_OUTPUT,
       .pull_down_en = GPIO_PULLDOWN_DISABLE,
       .pull_up_en = GPIO_PULLUP_DISABLE,
       .pin_bit_mask = (1ULL << USER_LED_NORMAL_FUNC)};

   gpio_config(&leds_config);
}