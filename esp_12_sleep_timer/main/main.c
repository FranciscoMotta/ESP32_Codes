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

/*
   MACROS
*/

/*
   VARIABLES
*/

/*
   DECLARACIÓN DE FUNCIONES
*/

/*
   PRINCIPAL
*/

void app_main(void)
{
   esp_sleep_enable_timer_wakeup(5000000); // Asignamos el sleep a 5seg
   printf("ESP en modo Sleep\n");
   uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM); // Esperamos a terminar el proceso
   uint64_t time_before = esp_timer_get_time(); // Tomamos el tiempo antes del modo sleep
   esp_light_sleep_start(); // Iniciamos el modo slepp
   uint64_t time_after = esp_timer_get_time(); // Tomamos el tiempo después del modo sleep
   printf("ESP-SLEEP: %lld\n", (time_after - time_before) / 1000); // Mostramos el tiempo que demoró el modo sleep
}

/*
   DEFINICION DE SUBFUNCIONES
*/