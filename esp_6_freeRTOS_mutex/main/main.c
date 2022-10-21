/* Inclusión de archivos */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/* Handlers */

SemaphoreHandle_t mutexBus;

/* Declaración de funciones */
void escribir_en_bus(char *mensaje);
void tarea1(void *params);
void tarea2(void *params);

/* Principal */
void app_main(void)
{
  mutexBus = xSemaphoreCreateMutex();
  xTaskCreate(&tarea1, "tarea1", 2048, NULL, 2, NULL);
  xTaskCreate(&tarea2, "tarea2", 2048, NULL, 2, NULL);
}

/* Definición de funciones */

void escribir_en_bus(char *mensaje)
{
  printf(mensaje);
}

void tarea1(void *params)
{
  while (true)
  {
    printf("Leyendo la temeperatura... \n");
    if (xSemaphoreTake(mutexBus, pdMS_TO_TICKS(1000)))
    {
      escribir_en_bus("La temperatura es: 25°C\n");
      xSemaphoreGive(mutexBus);
    }
    else
    {
      printf("Lectura temp. fuera de tiempo \n");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void tarea2(void *params)
{
  while (true)
  {
    printf("Leyendo la humedad... \n");
    if (xSemaphoreTake(mutexBus, pdMS_TO_TICKS(1000)))
    {
      escribir_en_bus("La humedad es: 50pc\n");
      xSemaphoreGive(mutexBus);
    }
    else
    {
      printf("Lectura hum. fuera de tiempo \n");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
