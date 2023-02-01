/*
    LIBRERIAS
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

/*
    MACROS
*/

#define LED_USER GPIO_NUM_2 // Led del usuario
#define TXD_PIN_US 5        // Pin asignado al transmiter
#define RXD_PIN_US 4        // Pin asignado al receiver
#define RX_BUFF_SIZE 1024   // Tamaño del bufer de recepcion

/*
    VARIABLES
*/

/*
    DECLARACIÓN DE FUNCIONES
*/

void User_Gpio_Init(void);
void User_Uart_Init(void);

/*
    PRINCIPAL
*/
void app_main(void)
{
    // Configuramos el led del usuario
    User_Gpio_Init();
    // Configuramos el uart
    User_Uart_Init();
    char mensaje[] = "ping";
    printf("Enviando: %s\n", mensaje);
    uart_write_bytes(UART_NUM_1, mensaje, sizeof(mensaje)); // Enviamos por uart

    char mensaje_entrante[RX_BUFF_SIZE];
    memset(mensaje_entrante, 0, sizeof(mensaje_entrante)); // Limpiamos el buffer
    uart_read_bytes(UART_NUM_1, mensaje_entrante, RX_BUFF_SIZE, pdMS_TO_TICKS(500));
    printf("Recibido: %s\n", mensaje_entrante);
    bool state_led = false;
    while (true)
    {
        /* code */
        state_led = !state_led;
        gpio_set_level(LED_USER, state_led);
        vTaskDelay(pdMS_TO_TICKS(100));
        memset(mensaje_entrante, 0, sizeof(mensaje_entrante)); // Limpiamos el buffer
        uart_read_bytes(UART_NUM_1, mensaje_entrante, RX_BUFF_SIZE, pdMS_TO_TICKS(500));
        printf("Recibido: %s\n", mensaje_entrante);
    }
}

/*
    DEFINICION DE SUBFUNCIONES
*/

void User_Uart_Init(void)
{
    uart_config_t uart_user =
        {
            .baud_rate = 9600,                     // Bits por segundo
            .data_bits = UART_DATA_8_BITS,         // Bits a transmitir
            .parity = UART_PARITY_DISABLE,         // Paridad
            .stop_bits = UART_STOP_BITS_1,         // Bit de parada
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // Control de flujo
            .source_clk = UART_SCLK_DEFAULT};
    uart_param_config(UART_NUM_1, &uart_user);                                                // Configuramos el uart 1 (0 es por d flashing an monitoring)
    uart_set_pin(UART_NUM_1, TXD_PIN_US, RXD_PIN_US, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); // Configuramos los pines
    uart_driver_install(UART_NUM_1, RX_BUFF_SIZE, 0, 0, NULL, 0);
}

void User_Gpio_Init(void)
{
    gpio_config_t led_user = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = (1ULL << LED_USER)};

    gpio_config(&led_user);
}