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

/*
   MACROS
*/

#define USER_LED_GPIO GPIO_NUM_2 // Led de la tarjeta del esp32
#define USER_SDA_GPIO 26         // Pin físico de la linea de datos
#define USER_SCL_GPIO 25         // Pin físico de la linea de reloj
#define DS1307_ADDRESS 0x68      // Dirección del RTC

/*
   VARIABLES
*/

/*
   DECLARACIÓN DE FUNCIONES
*/

void User_Gpio_Init(void);
void User_I2c_Init(void);

/*
   PRINCIPAL
*/

void app_main(void)
{
   // Configuramos el led del usuario
   User_Gpio_Init();
   // Configuramos el i2c del usuario
   User_I2c_Init();

   // Iniciamos la comunicación
   uint8_t raw[7];
   i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
   i2c_master_start(cmd_handle);
   i2c_master_write_byte(cmd_handle, (DS1307_ADDRESS << 1) | I2C_MASTER_READ, true);
   i2c_master_read(cmd_handle, (uint8_t *)&raw, 1, I2C_MASTER_ACK);
   i2c_master_stop(cmd_handle);
   i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
   i2c_cmd_link_delete(cmd_handle);
   printf("Segs: %x\n", raw[0]);
   bool state_led = false;
   uint8_t comando = 0x00;
   // uint8_t time_now[8] = {0x00, 0x00, 0x29, 0x18, 0x01, 0x31, 0x01, 0x23};
   // i2c_master_write_to_device(I2C_NUM_0, DS1307_ADDRESS, &time_now, sizeof(time_now), pdMS_TO_TICKS(1000));
   while (true)
   {
      /* code */
      state_led = !state_led;
      i2c_master_write_read_device(I2C_NUM_0,
                                   DS1307_ADDRESS,
                                   &comando,
                                   sizeof(comando),
                                   raw,
                                   sizeof(raw),
                                   pdMS_TO_TICKS(1000));
      ESP_LOG_BUFFER_HEX("DS1307", raw, sizeof(raw));
      printf("%x : %x : %x\n", raw[2], raw[1], raw[0]);
      gpio_set_level(USER_LED_GPIO, state_led);
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

/*
   DEFINICION DE SUBFUNCIONES
*/

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