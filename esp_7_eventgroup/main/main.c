/* Inclusión de archivos */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

/* Handlers */

EventGroupHandle_t evtGrp; // Creamos el manipulador de los eventos grupales

// Banderas de los eventos grupales

const int gotHTTP = BIT0; // (1 << 0) = 1
const int gotBLE = BIT1; // (1 << 1) = 2

/* Declaración de funciones */

void tomar_dato_HTTP(void *params);
void tomar_dato_BLE(void *params);
void tarea1(void *params);

/* Principal */
void app_main(void)
{
    // Instanciamos los eventos grupales
    evtGrp = xEventGroupCreate();

    // Se crean las tareas en el FreeRTOS
    xTaskCreate(&tomar_dato_BLE, "tomar BLE", 2048, NULL, 1, NULL);
    xTaskCreate(&tomar_dato_HTTP, "Tomar HTTP", 2048, NULL, 1, NULL);
    xTaskCreate(&tarea1, "Tarea 1", 2048, NULL, 1, NULL);
}

/* Definició de subfunciones */

void tomar_dato_HTTP(void *params)
{
    while (true)
    {
        xEventGroupSetBits(evtGrp, gotHTTP);
        printf("Tomar dato HTTP\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void tomar_dato_BLE(void *params)
{
    while (true)
    {
        xEventGroupSetBits(evtGrp, gotBLE);
        printf("Tomar dato BLE\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void tarea1 (void * params)
{
    while (true)
    {
        //                  evento   bits de interés  rstbts allbts  time
        xEventGroupWaitBits(evtGrp, gotHTTP | gotBLE, true, true, portMAX_DELAY);
        printf("Dato recibido de BLE y HTTP\n");
    }
}
