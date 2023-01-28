/*
    Inclusiones
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*
    Macros
*/

#define RS_LCD GPIO_NUM_26
#define EN_LCD GPIO_NUM_25

#define D4_LCD GPIO_NUM_13
#define D5_LCD GPIO_NUM_15
#define D6_LCD GPIO_NUM_14
#define D7_LCD GPIO_NUM_27

#define LED GPIO_NUM_2

#define LCD_PINES 6

#define LCD_COMMAND_CLEAR 0x01 // Lcd cleared
#define LCD_COMMAND_RETHO 0x02 // Lcd return to home (init conf)
#define LCD_COMMAND_ENTMS 0x06 // Lcd entry mode set
#define LCD_COMMAND_DONOF 0x0C // Lcd Disp ON Cursor ON and blink
#define LCD_COMMAND_DISHI 0x10 // Shift cursor to the left, AC is decreased by 1
#define LCD_COMMAND_FUSET 0x2C // LCD a 4 bits 2 lines 5x11 dots

typedef enum
{
    LCD_FIRST_LINE = 0,
    LCD_SECOND_LINE,
} _lines_to_lcd_t;

typedef enum
{
    LCD_COL_1 = 0,
    LCD_COL_2,
    LCD_COL_3,
    LCD_COL_4,
    LCD_COL_5,
    LCD_COL_6,
    LCD_COL_7,
    LCD_COL_8,
    LCD_COL_9,
    LCD_COL_10,
    LCD_COL_11,
    LCD_COL_12,
    LCD_COL_13,
    LCD_COL_14,
    LCD_COL_15,
    LCD_COL_16
} _column_to_lcd_t;

// Acciones de control del LCD

#define LCD_COMMAND_MODE()     \
    gpio_set_level(RS_LCD, 0); \
    vTaskDelay(pdMS_TO_TICKS(3)); // Modo comando
#define LCD_CHARACT_MODE()     \
    gpio_set_level(RS_LCD, 1); \
    vTaskDelay(pdMS_TO_TICKS(3)); // Modo caracter

/*
    Variables
*/

uint8_t gpio_lcd_gene_pines[] = {RS_LCD, EN_LCD, D4_LCD, D5_LCD, D6_LCD, D7_LCD};

uint8_t gpio_lcd_data_pines[] = {D4_LCD, D5_LCD, D6_LCD, D7_LCD};

uint8_t gpio_masks[] = {0x01, 0x02, 0x04, 0x08};
/*
    Declaración de funciones
*/

void Init_Lcd_Gpio(void);
void Write_Nibble_Lcd(char counter);
void Send_Nibble_Lcd(char dato);
void Send_Command_Lcd(char comando);
void Lcd_Init_Conf(void);
void Lcd_Set_Cursor(_lines_to_lcd_t line, _column_to_lcd_t column);
void Lcd_Send_Char(char dato);
void Lcd_Send_String(char *cadena);
void Lcd_Enable_Toggle(void);

/*
    Principal
*/

void app_main(void)
{
    Init_Lcd_Gpio(); // Initialize gpio to the display
    Lcd_Init_Conf(); // LCD is configurated to work with 4 bits
    Lcd_Set_Cursor(LCD_FIRST_LINE, LCD_COL_1);
    Lcd_Send_Char('a');
    uint8_t i = 0;
    bool state_led = false;
    while (true)
    {
        printf("Times: %i\n\r", i);
        i++;
        state_led = !state_led;
        gpio_set_level(GPIO_NUM_2, state_led);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/*
    Definición de funciones
*/

void Lcd_Send_String(char *cadena)
{
    uint8_t counter = 0;
    while (cadena[counter] != '\0')
    {
        Lcd_Send_Char(cadena[counter]);
        counter++;
    }
}

void Lcd_Send_Char(char dato)
{
    LCD_CHARACT_MODE();
    Send_Nibble_Lcd(dato);
}

void Lcd_Set_Cursor(_lines_to_lcd_t line, _column_to_lcd_t column)
{
    LCD_COMMAND_MODE();
    char pref = 0x00;
    if (line == LCD_FIRST_LINE)
    {
        pref = 0x80;
    }
    else
    {
        pref = 0xC0;
    }

    pref += column;
    Send_Nibble_Lcd(pref);
}

void Lcd_Init_Conf(void)
{
    Send_Command_Lcd(LCD_COMMAND_CLEAR);
    Send_Command_Lcd(LCD_COMMAND_RETHO);
    Send_Command_Lcd(LCD_COMMAND_ENTMS);
    Send_Command_Lcd(LCD_COMMAND_DONOF);
    Send_Command_Lcd(LCD_COMMAND_DISHI);
    Send_Command_Lcd(LCD_COMMAND_FUSET);
}

void Send_Command_Lcd(char comando)
{
    LCD_COMMAND_MODE();
    Send_Nibble_Lcd(comando);
}

void Send_Nibble_Lcd(char dato)
{
    char higher_bits = (dato & 0xF0) >> 4;
    char lower_bits = (dato & 0x0F);
    Write_Nibble_Lcd(higher_bits);
    Lcd_Enable_Toggle();
    Write_Nibble_Lcd(lower_bits);
    Lcd_Enable_Toggle();
}

void Write_Nibble_Lcd(char counter)
{
    uint8_t local_counter;
    for (local_counter = 0; local_counter < 4; local_counter++)
    {
        if (counter & gpio_masks[local_counter])
        {
            gpio_set_level(gpio_lcd_data_pines[local_counter], 1);
        }
        else
        {
            gpio_set_level(gpio_lcd_data_pines[local_counter], 0);
        }
    }
}

void Lcd_Enable_Toggle(void)
{
    gpio_set_level(EN_LCD, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(EN_LCD, 0);
}

void Init_Lcd_Gpio(void)
{
    // Iniciamos los pines del LCD como salida
    uint8_t counter_local;
    for (counter_local = 0; counter_local < LCD_PINES; counter_local++)
    {
        esp_rom_gpio_pad_select_gpio(gpio_lcd_gene_pines[counter_local]);
        gpio_set_direction(gpio_lcd_gene_pines[counter_local], GPIO_MODE_OUTPUT);
    }
    esp_rom_gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}