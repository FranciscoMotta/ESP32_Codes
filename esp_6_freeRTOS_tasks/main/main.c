/* Inclusión de archivos */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "math.h"

/* Enumeraciones */

enum
{
    CORE_1 = 0,
    CORE_2
};

/* Variables */

uint8_t data_UART_fake = 0;

/* Declaración de funciones */

void Fake_UART_Sender(void *params);
void Fake_UART_Decoder(void *params);

/* Principal */

static TaskHandle_t receiverHandler = NULL;

void app_main(void)
{
    xTaskCreatePinnedToCore(&Fake_UART_Decoder, "sender", 2048, "Core: 1", 2, &receiverHandler, CORE_1);
    xTaskCreatePinnedToCore(&Fake_UART_Sender, "receiver", 2048, "Core: 2", 2, NULL, CORE_2);
}

/* Definición de funciones */

void Fake_UART_Sender(void *params)
{
    while (true)
    {
        printf("Notificación enviada en: %s\n", (char *)params);
        int32_t dato_fake_UART_raw = esp_random(); // Recibimos un dato de 32bits random
        /* Como el teclando manda numeros de 8 bits pero solo hasta 64
        sacámos el módulo 64 del numero generado, el cual no puede superar los 8 bits,
        pero esos números son positivos y negativos, así que antes le sacamos el valor absoluto */
        int32_t dato_fake_UART_abs = abs(dato_fake_UART_raw); // Sacamos el valor absoluto
        data_UART_fake = dato_fake_UART_abs % 64;             // Sacamos el módulo
        printf("UART Falso convertido: %u \n", data_UART_fake);
        xTaskNotifyGive(receiverHandler);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void Fake_UART_Decoder(void *params)
{
    while (true)
    {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        uint8_t fake_Data_To_Decoder = data_UART_fake;
        data_UART_fake = 0;
        uint8_t higher_bits = (fake_Data_To_Decoder >> 3) & 0x07; // Nos quedamos con los 3 bits MSB
        uint8_t lower_bits = (fake_Data_To_Decoder & 0x07);       // Bits menos significativos
        printf("Notificación lista! en: %s\n", (char *)params);
        /*
            higher_bits = 0b00000ABC;
            lower_bits = 0b00000DEF;

            Para el primer servo: 0b000000AD;
            Para el segundo servo: 0b000000BE;
            Para el tercer servo: 0b000000CF;
        */
        uint8_t servo_1, servo_2, servo_3 = 0;
        servo_1 = (((higher_bits >> 1) & 0x02) | (lower_bits >> 2 & 0x01)) & 0x03;
        servo_2 = ((higher_bits & 0x02) | ((lower_bits & 0x02) >> 1)) & 0x03;
        servo_3 = (((higher_bits & 0x01) << 1) | (lower_bits & 0x01)) & 0x03;
        switch (servo_1)
        {
        case 0:
            printf("Servo 1: move: -- ");
            break;
        case 1:
            printf("Servo 1: move: -v ");
            break;
        case 2:
            printf("Servo 1: move: v- ");
            break;
        default:
            printf("Servo 1: move: vv ");
            break;
        }
        switch (servo_2)
        {
        case 0:
            printf("Servo 2: move: -- ");
            break;
        case 1:
            printf("Servo 2: move: -v ");
            break;
        case 2:
            printf("Servo 2: move: v- ");
            break;
        default:
            printf("Servo 2: move: vv ");
            break;
        }
        switch (servo_3)
        {
        case 0:
            printf("Servo 3: move: -- ");
            break;
        case 1:
            printf("Servo 3: move: -v ");
            break;
        case 2:
            printf("Servo 3: move: v- ");
            break;
        default:
            printf("Servo 3: move: vv ");
            break;
        }
        printf("\n");
    }
}