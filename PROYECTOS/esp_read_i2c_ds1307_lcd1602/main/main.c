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
#include "lcdi2c/lcd1602i2c.h"

/*
   MACROS
*/

#define USER_LED_GPIO GPIO_NUM_2 // Led de la tarjeta del esp3

#define USER_SDA_GPIO 26         // Pin físico de la linea de datos
#define USER_SCL_GPIO 25         // Pin físico de la linea de reloj
#define DS1307_ADDRESS 0x68      // Dirección del RTC
#define INITIAL_ADDRESS 0x00     // Dirección inicial


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
}_format_hours_ds1307_t;

/*
   DECLARACIÓN DE FUNCIONES
*/

void User_Gpio_Init(void);
void User_I2c_Init(void);
void User_I2c_Get_Date(_format_hours_ds1307_t format);

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
   // Luz de fondo encendida
   Lcd_I2c_Backlight_Status(BACK_LIGHT_ON);
   // Iniciamos el LCD
   // Lcd_I2c_Init();
   // Iniciamos la comunicación
   bool state_led = false;

   while (true)
   {
      /* code */
      state_led = !state_led;
      User_I2c_Get_Date(FORMAT_24_H);
      printf("hora: %x%xh:%x%xm:%x%xs\n\r", dhours, uhours, dmins, umins, dsegs, usegs);
      gpio_set_level(USER_LED_GPIO, state_led);
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

/*
   i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
   i2c_master_start(cmd_handle);
   i2c_master_write_byte(cmd_handle, (DS1307_ADDRESS << 1) | I2C_MASTER_READ, true);
   i2c_master_read(cmd_handle, (uint8_t *)&raw, 1, I2C_MASTER_ACK);
   i2c_master_stop(cmd_handle);
   i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
   i2c_cmd_link_delete(cmd_handle);
   printf("Segs: %x\n", raw[0]);
*/

/*
   DEFINICION DE SUBFUNCIONES
*/

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