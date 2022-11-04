/* Inclusión de archivos */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

/* Macros */

#define ELEMENTS_ON_QUEUE 3

/* Handlers */
QueueHandle_t queue; // Creamos el manipulador de la cola

/* Declaración de funciones */
void listen_For_HTTPS(void *params);
void tarea1(void *params);

/* Principal */
void app_main(void)
{
  // Crear la cola
  queue = xQueueCreate(ELEMENTS_ON_QUEUE, sizeof(int));
  // Crear las tareas
  xTaskCreate(&listen_For_HTTPS, "tomar HTTPS", 2048, NULL, 2, NULL);
  xTaskCreate(&tarea1, "Hacer algo con el HTTPS", 2048, NULL, 1, NULL);
}

/* Definición de funciones */

void listen_For_HTTPS(void *params)
{
  int count = 0;
  while (true)
  {
    count -= -1;
    printf("Mensaje por HTTPS recibido\n");
    /*Mandar cola:
                         handler  dir. Item    time*/
    long ok = xQueueSend(queue, &count, pdMS_TO_TICKS(1000));
    if (ok)
    {
      printf("Adjuntado a cola\n");
    }
    else
    {
      printf("No adjuntado a cola\n");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void tarea1(void *params)
{
  while (true)
  {
    int rx_Int = 0;
    if (xQueueReceive(queue, &rx_Int, pdMS_TO_TICKS(1000)))
    {
      printf("Realizando tarea 1: %d\n", rx_Int);
    }
  }
}