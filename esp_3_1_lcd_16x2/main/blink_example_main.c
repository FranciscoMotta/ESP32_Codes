/*LIBRERÍAS*/
#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/*MACROS*/

#define LOW 0
#define HIGH 1

// PINES DE CONTROL

#define LCD_1602_RS 22
#define LCD_1602_EN 23

// MODOS DE CONTROL

#define LCD_1602_MODE_CONF 0
#define LCD_1602_MODE_ESCR 1

// PINES DE DATOS

#define LCD_1602_D0 13
#define LCD_1602_D1 12
#define LCD_1602_D2 14
#define LCD_1602_D3 27
#define LCD_1602_D4 26
#define LCD_1602_D5 25
#define LCD_1602_D6 33
#define LCD_1602_D7 32

#define LCD_1602_NUM_PINES 10
#define LCD_1602_DATA_PINES 8

uint8_t lcd_1602_pines[10] = {
    LCD_1602_RS, LCD_1602_EN, LCD_1602_D0,
    LCD_1602_D1, LCD_1602_D2, LCD_1602_D3,
    LCD_1602_D4, LCD_1602_D5, LCD_1602_D6,
    LCD_1602_D7};

uint8_t lcd_1602_data_pines[8] = {
    LCD_1602_D0, LCD_1602_D1, LCD_1602_D2, LCD_1602_D3,
    LCD_1602_D4, LCD_1602_D5, LCD_1602_D6, LCD_1602_D7};

/*DECLARACIÓN DE SUBFUNCIONES*/

void lcd_1602_pines_conf(void);
void lcd_1602_set_mode(uint8_t mode);
void lcd_1602_send_8_bits(char byte);
void lcd_1602_enable_toggle(void);

/*PRINCIPAL*/
void app_main(void)
{
    lcd_1602_pines_conf(); // Pines configurados como salida
    /*INICIAMOS LA CONF DEL LCD*/
    lcd_1602_set_mode(LCD_1602_MODE_CONF);
    lcd_1602_send_8_bits(0x01);
    lcd_1602_enable_toggle();
    lcd_1602_send_8_bits(0x03);
    lcd_1602_enable_toggle();
    lcd_1602_send_8_bits(0x07);
    lcd_1602_enable_toggle();
    lcd_1602_send_8_bits(0x0F);
    lcd_1602_enable_toggle();
    lcd_1602_send_8_bits(0x1C);
    lcd_1602_enable_toggle();
    lcd_1602_set_mode(LCD_1602_MODE_ESCR);
    lcd_1602_send_8_bits("H");
    lcd_1602_enable_toggle();
    while (1)
    {
    }
}

/*DEFINICIÓN DE SUBFUNCIONES*/

void lcd_1602_pines_conf(void)
{
    uint8_t contador_pines;
    for (contador_pines = 0; contador_pines < LCD_1602_NUM_PINES; contador_pines++)
    {
        esp_rom_gpio_pad_select_gpio(lcd_1602_pines[contador_pines]);         // Se elige los pines
        gpio_set_direction(lcd_1602_pines[contador_pines], GPIO_MODE_OUTPUT); // Pines como salida
        gpio_set_level(lcd_1602_pines[contador_pines], LOW);                  // Pines apagados
    }
}

void lcd_1602_set_mode(uint8_t mode)
{
    if (mode == 0)
    {
        gpio_set_level(LCD_1602_RS, LOW);
    }
    else if (mode == 1)
    {
        gpio_set_level(LCD_1602_RS, HIGH);
    }
    else
    {
        // nop();
    }
}
void lcd_1602_send_8_bits(char byte)
{
    char second_mask = 0x00;
    for (int bit_counter = 0; bit_counter < 8; bit_counter++)
    {
        second_mask |= (1 << bit_counter);
        if ((byte & second_mask) != 0)
        {
            gpio_set_level(lcd_1602_data_pines[bit_counter], HIGH);
        }
        else
        {
            gpio_set_level(lcd_1602_data_pines[bit_counter], LOW);
        }
        second_mask = 0;
    }
}

void lcd_1602_enable_toggle(void)
{
    gpio_set_level(LCD_1602_EN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(3));
    gpio_set_level(LCD_1602_EN, LOW);
}
