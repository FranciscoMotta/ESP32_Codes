/* Inclusión de archivos */

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_timer.h>
#include <esp_system.h>

/* Declaración de funciones */

void on_timer(TimerHandle_t xtimer);

/* Principal */

void app_main(void)
{
    printf("App Started %lli\n", esp_timer_get_time() / 1000);
    //                                   nombre          tiempo         reset         callback
    TimerHandle_t xTimer = xTimerCreate("My timer", pdMS_TO_TICKS(1000), true, NULL, on_timer);
    // Iniciamos el timer y le pasamos el identificador
    xTimerStart(xTimer, 0);
}

/* Definición de funciones */

void on_timer(TimerHandle_t xtimer)
{
    printf("Time to hit %lli\n", esp_timer_get_time() / 1000);
}
