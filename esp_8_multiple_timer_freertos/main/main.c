/* Inclusión de archivos */
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_timer.h>
#include <esp_system.h>

/* Declaración de funciones */

void on_timer1(TimerHandle_t xtimer1);
void on_timer2(TimerHandle_t xtimer2);
void on_timer3(TimerHandle_t xtimer3);

/* código principal principal */
void app_main(void)
{
    /* Creamos los timers */
    TimerHandle_t xtimer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(500), true, NULL, on_timer1);
    TimerHandle_t xtimer2 = xTimerCreate("Timer2", pdMS_TO_TICKS(1000), true, NULL, on_timer2);
    TimerHandle_t xtimer3 = xTimerCreate("Timer3", pdMS_TO_TICKS(2000), true, NULL, on_timer3);

    /* Iniciamos los timers */
    xTimerStart(xtimer1, 0);
    xTimerStart(xtimer2, 0);
    xTimerStart(xtimer3, 0);
}

/* Definición de funciones */

void on_timer1(TimerHandle_t xtimer1)
{
    printf("Timer: 1 - fired! timer: %lld\n\r", esp_timer_get_time()/1000);
}

void on_timer2(TimerHandle_t xtimer2)
{
    printf("Timer: 2 - fired! timer: %lld\n\r", esp_timer_get_time()/1000);
}

void on_timer3(TimerHandle_t xtimer3)
{
    printf("Timer: 3 - fired! timer: %lld\n\r", esp_timer_get_time()/1000);
}