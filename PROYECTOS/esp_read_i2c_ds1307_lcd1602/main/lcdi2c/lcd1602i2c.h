#ifndef LCD1602I2C_H
#define LCD1602I2C_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_types.h"
#include "lcdi2c/lcd1602i2c.h"


#define LCDI2C_ADDRESS             0x27      // Dirección del L2C1602I2C
#define LCDI2C_CLEAR               0x01        // replace all characters with ASCII 'space'
#define LCDI2C_HOME                0x02        // return cursor to first position on first line
#define LCDI2C_ENTRY_MODE          0x06        // shift cursor from left to right on read/write
#define LCDI2C_DISPLAY_OFF         0x08        // turn display off
#define LCDI2C_DISPLAY_ON          0x0C        // display on, cursor off, don't blink character
#define LCDI2C_FUNCTION_RESET      0x30        // reset the LCD
#define LCDI2C_FUNCTION_SET        0x28        // 4-bit data, 2-line display, 5 x 7 font
#define LCDI2C_SET_CURSOR          0x80        // set cursor position

// Lista de pines
// B0 -> RS
// B1 -> RW
// B2 -> E
// B3 -> Backlight
// B4 -> D4
// B5 -> D5
// B6 -> D6
// B7 -> D7

/* Función para encender o apagar el backlight del LCD */

// Estado del back light
typedef enum 
{
    BACK_LIGHT_ON = 0,
    BACK_LIGHT_OFF
}_back_light_status_t;

bool general_back_light_var; // Para manejo general del backlight

void Lcd_I2c_Backlight_Status (_back_light_status_t status);

/* Función para inciar el LCD */

typedef enum 
{
    MODO_COMANDO = 0,
    MODO_CARACTER
}_mode_lcd_control_t;

void Lcd_I2c_Send_Byte_With_Toggle_Enable (char data, _mode_lcd_control_t modo);

/* Funcion para inicializar el LCD */

void Lcd_I2c_Init (void);

#endif