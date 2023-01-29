/*
    Inclusiones
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>

/*
    Macros
*/

#define LED GPIO_NUM_2
#define BUTTON GPIO_NUM_15

/*
    Rutina de servicio a interrupcion
*/

// Creamos una cola en FreeRTOS
QueueHandle_t interruptQueue;

// Este código se corre en la DRAM -> especial para interrupciones
static void IRAM_ATTR gpio_isr_handeler (void *args)
{
 /*
 Acá se busca mantener el código simple, corto y salir los 
 más rápido que se pueda, no es para código complejo.
 */
    int pin = (int)args;
    xQueueSendFromISR(interruptQueue, &pin, NULL);
}

void buttonPressTask(void *params)
{
    int pinNumber, count = 0;
    while (true)
    {
        /* code */
        if (xQueueReceive(interruptQueue, &pinNumber, portMAX_DELAY))
        {
            /* code */
            printf("GPIO %n was pressed %d times. The state is %d\n", &pinNumber, count++, gpio_get_level(BUTTON));
        }
        
    }
    
}

/*
    Principal
*/

void app_main(void)
{
    // Configuramos el LED
    esp_rom_gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);

    // Configuramos el boton
    esp_rom_gpio_pad_select_gpio(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_pulldown_en(BUTTON);
    // Asignamos la interrupción
    gpio_set_intr_type(BUTTON, GPIO_INTR_POSEDGE);

    // Creamos la cola (Queue)
    interruptQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(buttonPressTask, "Boton presionado", 2048, NULL, 1, NULL);
    // Asignamos la función de servicio a la interrupcion
    gpio_install_isr_service(0);
    // Asignamos un handler
    gpio_isr_handler_add(BUTTON, gpio_isr_handeler, (void *) BUTTON);
    while (true)
    {
        /* code */
        vTaskDelay(1);
    }
    
}
