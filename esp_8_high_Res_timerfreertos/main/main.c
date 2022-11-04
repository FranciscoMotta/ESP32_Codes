/* Inclusión de archivos */

#include <stdio.h>
#include <esp_timer.h>
#include <driver/gpio.h>

/* Declaración de funciones */

void timer_callback(void * args);

/* Main */
void app_main(void)
{
    esp_rom_gpio_pad_select_gpio(GPIO_NUM_2); // Definimos el GPIO de uso
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT); // Ponemos el GPIO como salida
    /* Definimos los argumentos del timer */
    const esp_timer_create_args_t esp_timer_create_args = {
        .callback = timer_callback, // Función de callback
        .name = "mi temporizador" // Nombre del timer
    };
    esp_timer_handle_t esp_timer_handle; // Manipulador
    esp_timer_create(&esp_timer_create_args, &esp_timer_handle); // Creamis el timer
    esp_timer_start_periodic(esp_timer_handle, 50); // Iniciamos el timer una vez
}

/* Definición de funciones */

void timer_callback(void * args)
{
    static bool led_state;
    led_state = ! led_state;
    gpio_set_level(GPIO_NUM_2, led_state);
}
