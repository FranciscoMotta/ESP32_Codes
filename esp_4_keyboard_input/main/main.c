#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    char c = 0;    // Caracter
    char str[100]; // Cadena
    memset(str, 0, sizeof(str));
    while (c != '\n')
    {
        c = getchar(); // Tomamos un dato del teclado
        if (c != 0xff)
        {
            str[strlen(str)] = c;
            printf("%c", c);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    printf("you typed: %s\n", str);
}
