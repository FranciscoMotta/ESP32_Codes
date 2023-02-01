/*
   LIBRERIAS
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_types.h"
// #include "lcd1602i2c.h"

/*
   MACROS
*/

#define USER_LED_GPIO GPIO_NUM_2 // Led de la tarjeta del esp3

#define USER_SDA_GPIO 26     // Pin físico de la linea de datos
#define USER_SCL_GPIO 25     // Pin físico de la linea de reloj
#define DS1307_ADDRESS 0x68  // Dirección del RTC
#define INITIAL_ADDRESS 0x00 // Dirección inicial

#define LCDI2C_ADDRESS 0x27        // DirecciÃ³n del L2C1602I2C
#define LCDI2C_CLEAR 0x01          // replace all characters with ASCII 'space'
#define LCDI2C_HOME 0x02           // return cursor to first position on first line
#define LCDI2C_ENTRY_MODE 0x06     // shift cursor from left to right on read/write
#define LCDI2C_DISPLAY_OFF 0x08    // turn display off
#define LCDI2C_DISPLAY_ON 0x0C     // display on, cursor off, don't blink character
#define LCDI2C_FUNCTION_RESET 0x30 // reset the LCD
#define LCDI2C_FUNCTION_SET 0x28   // 4-bit data, 2-line display, 5 x 7 font
#define LCDI2C_SET_CURSOR 0x80     // set cursor position

#define RS_BIT 0
#define RW_BIT 1
#define EN_BIT 2
#define BL_BIT 3
#define D4_BIT 4
#define D5_BIT 5
#define D6_BIT 6
#define D7_BIT 7

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY(byte)      \
   (byte & 0x80 ? '1' : '0'),     \
       (byte & 0x40 ? '1' : '0'), \
       (byte & 0x20 ? '1' : '0'), \
       (byte & 0x10 ? '1' : '0'), \
       (byte & 0x08 ? '1' : '0'), \
       (byte & 0x04 ? '1' : '0'), \
       (byte & 0x02 ? '1' : '0'), \
       (byte & 0x01 ? '1' : '0')

/*
   VARIABLES
*/

uint8_t raw[7];
uint8_t comando = INITIAL_ADDRESS;
volatile char usegs = 0, dsegs = 0;
volatile char umins = 0, dmins = 0;
volatile char uhours = 0, dhours = 0;

typedef enum
{
   FORMAT_24_H = 0,
   FORMAT_12_H
} _format_hours_ds1307_t;

typedef enum
{
   BACK_LIGHT_OFF = 0,
   BACK_LIGHT_ON
} _back_light_status_t;

bool general_back_light_var; // Para manejo general del backlight

typedef enum
{
   MODO_COMANDO = 0,
   MODO_CARACTER
} _mode_lcd_control_t;

typedef enum
{
   COL_1 = 0,
   COL_2,
   COL_3,
   COL_4,
   COL_5,
   COL_6,
   COL_7,
   COL_8,
   COL_9,
   COL_10,
   COL_11,
   COL_12,
   COL_13,
   COL_14,
   COL_15,
   COL_16
} _column_lcd_t;

typedef enum
{
   ROW_1 = 0,
   ROW_2
} _row_lcd_t;

/*
   DECLARACIÓN DE FUNCIONES
*/

// Lista de pines
// B0 -> RS
// B1 -> RW
// B2 -> E
// B3 -> Backlight
// B4 -> D4
// B5 -> D5
// B6 -> D6
// B7 -> D7

void User_Gpio_Init(void);                                                      // Funcion para inicializar el GPIO
void User_I2c_Init(void);                                                       // Funcion para inicializar el protocolo i2c
void User_I2c_Get_Date(_format_hours_ds1307_t format);                          // Funcion para obtener la hora-fecha del RTC
void Lcd_I2c_Write_Nibble(char data);                                           // Funcion para escribir un nibble para el lcd - 4b
void Lcd_I2c_Send_Byte_With_Toggle_Enable(char data, _mode_lcd_control_t modo); // Mandamos un dato con todo y en-togg
void Lcd_I2c_Backlight_Status(_back_light_status_t status);                     // Funcion para prender o apagar el backlight
void Lcd_I2c_Init(void);                                                        // Funcion para iniciar el lcd
void Lcd_I2c_Write_Character(char caracter);                                    // Funcion para escribir un caracter
void Lcd_I2c_Write_String(char *cadena);                                        // Funcion para escribir una cadena
void Lcd_I2c_Set_Cursor(_row_lcd_t fila, _column_lcd_t columna);                // Funcion para ubicar el cursor del lcd
void Lcd_I2c_Show_Date(void);
/*
   PRINCIPAL
*/

void app_main(void)
{
   // Configuramos el led del usuario
   User_Gpio_Init();
   // Configuramos el i2c del usuario
   User_I2c_Init();
   /* Configuraciones del LCD */
   // Iniciamos el LCD
   // Luz de fondo encendida
   Lcd_I2c_Init();
   bool state_led = false;
   uint8_t counter = 0;
   while (true)
   {
      /* code */
      state_led = !state_led;
      gpio_set_level(USER_LED_GPIO, state_led);
      User_I2c_Get_Date(FORMAT_24_H);
      Lcd_I2c_Show_Date();
      vTaskDelay(pdMS_TO_TICKS(100));
   }
}

/*
   DEFINICION DE SUBFUNCIONES
*/

void Lcd_I2c_Show_Date(void)
{
   /*
      Hora
   */
   Lcd_I2c_Set_Cursor(ROW_1, COL_1);
   Lcd_I2c_Write_String("Hor: ");
   Lcd_I2c_Set_Cursor(ROW_1, COL_6);
   Lcd_I2c_Write_Character(dhours + 0x30);
   Lcd_I2c_Write_Character(uhours + 0x30);
   Lcd_I2c_Write_Character(':');
   Lcd_I2c_Write_Character(dmins + 0x30);
   Lcd_I2c_Write_Character(umins + 0x30);
   Lcd_I2c_Write_Character(':');
   Lcd_I2c_Write_Character(dsegs + 0x30);
   Lcd_I2c_Write_Character(usegs + 0x30);
   Lcd_I2c_Write_Character(' ');
   if (((dhours * 10) + uhours) > 12)
   {
      Lcd_I2c_Write_Character('p');
      Lcd_I2c_Write_Character('m');
   }
   else
   {
      Lcd_I2c_Write_Character('a');
      Lcd_I2c_Write_Character('m');
   }

   /* Fecha */
}

void Lcd_I2c_Set_Cursor(_row_lcd_t fila, _column_lcd_t columna)
{
   uint8_t pos_cur = 0x00;
   if (fila == ROW_1)
   {
      /* code */
      pos_cur = 0x80;
   }
   else if (fila == ROW_2)
   {
      /* code */
      pos_cur = 0xC0;
   }
   else
   {
      /* code */
   }
   pos_cur += columna;
   Lcd_I2c_Send_Byte_With_Toggle_Enable(pos_cur, MODO_COMANDO);
}

void Lcd_I2c_Write_String(char *cadena)
{
   uint8_t contador_cadena = 0;
   while (cadena[contador_cadena] != '\0')
   {
      /* code */
      Lcd_I2c_Write_Character(cadena[contador_cadena]);
      contador_cadena++;
   }
}

void Lcd_I2c_Write_Character(char caracter)
{
   Lcd_I2c_Send_Byte_With_Toggle_Enable(caracter, MODO_CARACTER);
}

/* FunciÃ³n para encender o apagar el backlight del LCD */

void Lcd_I2c_Backlight_Status(_back_light_status_t status)
{
   uint8_t com = (status << BL_BIT);
   if (com & 0x08)
   {
      general_back_light_var = 1;
   }
   else
   {
      general_back_light_var = 0;
   }

   i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &com, sizeof(com), portMAX_DELAY);
}

/* Funcion para inicializar el LCD */

void Lcd_I2c_Init(void)
{
   Lcd_I2c_Backlight_Status(BACK_LIGHT_ON);
   Lcd_I2c_Send_Byte_With_Toggle_Enable(0x01, MODO_COMANDO);
   Lcd_I2c_Send_Byte_With_Toggle_Enable(0x02, MODO_COMANDO);
   Lcd_I2c_Send_Byte_With_Toggle_Enable(0x06, MODO_COMANDO);
   Lcd_I2c_Send_Byte_With_Toggle_Enable(0x0C, MODO_COMANDO);
   Lcd_I2c_Send_Byte_With_Toggle_Enable(0x28, MODO_COMANDO);
   Lcd_I2c_Send_Byte_With_Toggle_Enable(0x80, MODO_COMANDO);
}

void Lcd_I2c_Send_Byte_With_Toggle_Enable(char data, _mode_lcd_control_t modo)
{
   /* Construimos los Nibles a mandar*/
   uint8_t higher_bits = (data & 0xF0);     // Los 4 bits MSB
   uint8_t lower_bits = (data & 0x0F) << 4; // Los 4 bits LSB

   // Cargamos el modo al cual se envian los nibles

   if (modo == MODO_COMANDO)
   {
      higher_bits &= ~(1 << RS_BIT); // El & 0x01 es una contigencia provicinal
      lower_bits &= ~(1 << RS_BIT);
   }
   else if (modo == MODO_CARACTER)
   {
      higher_bits |= (1 << RS_BIT); // El & 0x01 es una contigencia provicinal
      lower_bits |= (1 << RS_BIT);
   }

   // Para esta prueba, dejaremos el rw en 0 siempre por lo tanto

   uint8_t rw_state = 0;
   higher_bits &= ~(1 << RW_BIT);
   lower_bits &= ~(1 << RW_BIT);

   // Depende de la configuraciÃ³n del backlight lo tomamos como referencia

   if (general_back_light_var == 1)
   {
      /* code */
      higher_bits |= (1 << BL_BIT);
      lower_bits |= (1 << BL_BIT);
   }
   else
   {
      /* code */
      higher_bits &= ~(1 << BL_BIT);
      lower_bits &= ~(1 << BL_BIT);
   }

   // Ahora mandamos los datos por el i2c con los bytes construidos

   // Mandamos los 4 MSB
   higher_bits |= (1 << EN_BIT);
   i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &higher_bits, 1, pdMS_TO_TICKS(1000));
   esp_rom_delay_us(50);
   higher_bits &= ~(1 << EN_BIT);
   i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &higher_bits, 1, pdMS_TO_TICKS(1000));
   vTaskDelay(pdMS_TO_TICKS(10));

   // Mandamos los 4 LSB
   lower_bits |= (1 << EN_BIT);
   i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &lower_bits, 1, pdMS_TO_TICKS(1000));
   esp_rom_delay_us(50);
   lower_bits &= ~(1 << EN_BIT);
   i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &lower_bits, 1, pdMS_TO_TICKS(1000));
   vTaskDelay(pdMS_TO_TICKS(10));

   /* Limpiamos las variables */

   higher_bits = 0x00;
   lower_bits = 0x00;
}

void User_I2c_Get_Date(_format_hours_ds1307_t format)
{
   i2c_master_write_read_device(I2C_NUM_0,
                                DS1307_ADDRESS,
                                &comando,
                                sizeof(comando),
                                raw,
                                sizeof(raw),
                                pdMS_TO_TICKS(1000));

   char hours = raw[2];
   char mins = raw[1];
   char segs = raw[0];

   if ((hours > 12) && format)
   {
      hours %= 12;
   }

   dsegs = (segs & 0x70) >> 4;
   usegs = (segs & 0x0F);

   dmins = (mins & 0x70) >> 4;
   umins = (mins & 0x0F);

   dhours = (hours & 0x30) >> 4;
   uhours = (hours & 0x0F);

   printf("hora: %x%xh:%x%xm:%x%xs\n\r", dhours, uhours, dmins, umins, dsegs, usegs);
}

void User_I2c_Init(void)
{
   i2c_config_t i2c_user =
       {
           .mode = I2C_MODE_MASTER,             // Modo maestro
           .scl_io_num = USER_SCL_GPIO,         // Pin de reloj
           .sda_io_num = USER_SDA_GPIO,         // Pin de datos
           .scl_pullup_en = GPIO_PULLUP_ENABLE, // Activamos el pullup de la linea de reloj
           .sda_pullup_en = GPIO_PULLUP_ENABLE, // Activamos el pullup de la linea de datos
           .master.clk_speed = 100000           // Velocidad de transmisión
       };

   i2c_param_config(I2C_NUM_0, &i2c_user); // Configuramos el i2c
   i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void User_Gpio_Init(void)
{
   gpio_config_t led_user = {
       .mode = GPIO_MODE_OUTPUT,
       .intr_type = GPIO_INTR_DISABLE,
       .pull_down_en = GPIO_PULLDOWN_DISABLE,
       .pull_up_en = GPIO_PULLUP_DISABLE,
       .pin_bit_mask = (1ULL << USER_LED_GPIO)};
   gpio_config(&led_user);
}