/*
   LIBRERIAS
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "driver/adc.h"

/*
   MACROS
*/

#define USER_BUT_JOY GPIO_NUM_15
#define USER_ADC_VRX ADC_CHANNEL_0
#define USER_ADC_VRY ADC_CHANNEL_3

#define USER_SPI_CS GPIO_NUM_5
#define USER_SPI_MOSI GPIO_NUM_21
#define USER_SPI_MISO GPIO_NUM_19
#define USER_SPI_CLK GPIO_NUM_18
#define MAX7219E_HOST VSPI_HOST
#define USER_SPI_PIN_NO_USED -1
#define SIZE_TO_SHARE_SPI 32

// DEFINICIONES DEL MACROS DEL MAX7219

#define REGISTER_NO_OP 0x00
#define REGISTER_DECODE_MODE 0x09
#define REGISTER_INTENSITY 0x0A
#define REGISTER_SCAN_LIMIT 0x0B
#define REGISTER_SHUTDOWN 0x0C
#define REGISTER_DISPLAY_TEST 0x0F

/*
   PINES DEL PROTOCOLO SPI
   Chip Select (CS) -> GPIO 5
   Master Output Slave Input (MOSI) -> GPIO 21
   Master Input Slave Output (MISO) -> GPIO 19
   Clock (SCK - CLK) -> GPIO 18
*/

/*
   VARIABLES
*/

/*
Los datos hacia el MAX7219 se envían de la siguiente manera
Trama:
LSB                                                 MSB
D0 D1 D2 D3 D4 D5 D6 D7 | D8 D9 D10 D11 D12 D13 D14 D15
          DATA              ADDRESS      X   X   X   X

Las direcciones (Address) son:
     D8 D9 D10 D11  D12 D13 D14 D15
         ADDRESS      X   X   X   X
    _______________________________
   |    ADDRESS    |      FUNC     |
   |_______________________________|
   |    0xX0       |      nop      |
   |_______________|_______________|
   |    0xX1       |     Dig 1     |
   |_______________|_______________|
   |    0xX2       |     Dig 2     |
   |_______________|_______________|
   |    0xX3       |     Dig 3     |
   |_______________|_______________|
   |    0xX4       |     Dig 4     |
   |_______________|_______________|
   |    0xX5       |     Dig 5     |
   |_______________|_______________|
   |    0xX6       |     Dig 6     |
   |_______________|_______________|
   |    0xX7       |     Dig 7     |
   |_______________|_______________|
   |    0xX8       |     Dig 8     |
   |_______________|_______________|
   |    0xX9       |  Decode Mode  |
   |_______________|_______________|
   |    0xXA       |  Intensidad   |
   |_______________|_______________|
   |    0xXB       |  Scan Limit   |
   |_______________|_______________|
   |    0xXC       |   Shutdown    |
   |_______________|_______________|
   |    0xXC       |   Shutdown    |
   |_______________|_______________|

*/

typedef enum
{
   SHUTDOWN_MODE = 0x00,
   NORMAL_OPERATION_SHUTDOWN = 0x01 // Enumerar esto no es necesario
} _shutdown_command_mode_t;

typedef enum
{
   NO_DECODE_FOR_DIGITS_7_0 = 0x00,
   CODE_B_DECO_0_NODECO_7_1 = 0x01, // Enumerar esto no es necesario
   CODE_B_DECO_3_0_NODECO_7_4 = 0x0F,
   CODE_B_DECODE_7_4_BITS = 0xFF
} _decode_command_mode_t;

/*
Esto depende del integrado MAX72XX que se esté usando
en este caso, usamos el MAX7219, revisar: Tabla 7 - Pag. 9
https://www.sparkfun.com/datasheets/Components/General/COM-09622-MAX7219-MAX7221.pdf
*/

typedef enum
{
   DUTY_CICLE_1_32 = 0x00,
   DUTY_CICLE_3_32,
   DUTY_CICLE_5_32,
   DUTY_CICLE_7_32,
   DUTY_CICLE_9_32,
   DUTY_CICLE_11_32,
   DUTY_CICLE_13_32,
   DUTY_CICLE_15_32,
   DUTY_CICLE_17_32,
   DUTY_CICLE_19_32,
   DUTY_CICLE_21_32,
   DUTY_CICLE_23_32,
   DUTY_CICLE_25_32,
   DUTY_CICLE_27_32,
   DUTY_CICLE_29_32,
   DUTY_CICLE_31_32
} _intensity_command_mode_t;

typedef enum
{
   DISPLAY_DIG_0_ONLY = 0x00,
   DISPLAY_DIG_0_1,
   DISPLAY_DIG_0_2,
   DISPLAY_DIG_0_3,
   DISPLAY_DIG_0_4,
   DISPLAY_DIG_0_5,
   DISPLAY_DIG_0_6,
   DISPLAY_DIG_0_7
} _scan_limit_command_mode_t;

typedef enum
{
   NORMAL_OPERATION_DISPLAY_TEST = 0x00,
   DISPLAY_TEST_MODE = 0x01 // Enumerar esto no es necesario
} _display_test_command_mode_t;

typedef struct
{
   _shutdown_command_mode_t shutdown;
   _decode_command_mode_t decode;
   _intensity_command_mode_t intensity;
   _scan_limit_command_mode_t scan_limit;
   _display_test_command_mode_t display_test;

} _max7219_func_config_t;

volatile uint8_t contador = 1;

/*
   DECLARACIÓN DE FUNCIONES
*/

void User_Gpio_Init(void);
void User_Spi_Init(void);

// FUNCIONES PROPIAS DEL MAX7219

static void Max7219_Write_Register(uint8_t registro, uint8_t dato);
static void Max7219_Set_Row(uint8_t row);
static void Max7219_Set_Col(uint8_t col);
static void Max7219_Clear_Display(void);
static void Max7219_Config_Register(_max7219_func_config_t *config);
static void Max7219_Init(void);

// Funciones para el ADC

void User_Adc_Init(void);

// Funciones del movimiento
uint8_t check_mov_x(int pos_x);
uint8_t check_mov_y(int pos_y);
void joystick_but_op(uint8_t boton);

void ojo_up(void);
void ojo_down(void);

/*
   PRINCIPAL
*/

spi_device_handle_t spi;

// Graficos de modelo

uint8_t point[8] = {0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00};
uint8_t hearth[8] = {0x78, 0x84, 0x82, 0x41, 0x41, 0x82, 0x84, 0x78};

// Principal

void app_main(void)
{
   // Iniciamos el GPIO del usuario
   User_Gpio_Init();
   // Iniciamos el ADC
   User_Adc_Init();
   // Iniciamos el protocolo del SPI
   User_Spi_Init();
   // Iniciamos el MAX
   Max7219_Init();
   // Variables de trabajo
   Max7219_Clear_Display();
   // Bucle
   uint8_t boton_state;
   while (true)
   {
      /* code */
      boton_state = gpio_get_level(USER_BUT_JOY);
      printf("B: %d\n\r", boton_state);
      joystick_but_op(boton_state);
      Max7219_Clear_Display();
      Max7219_Write_Register(check_mov_x(adc1_get_raw(USER_ADC_VRX)) - 1, check_mov_y(adc1_get_raw(USER_ADC_VRY)));
      Max7219_Write_Register(check_mov_x(adc1_get_raw(USER_ADC_VRX)), check_mov_y(adc1_get_raw(USER_ADC_VRY)));
      vTaskDelay(pdMS_TO_TICKS(100));
   }
}

/*
   DEFINICION DE SUBFUNCIONES
*/

void joystick_but_op(uint8_t boton)
{
   if (!boton)
   {
      for (int i = 0; i < 5; i++)
      {
         Max7219_Clear_Display();
         for (int i = 1; i < 9; i++)
         {
            /* code */
            Max7219_Write_Register(i, hearth[i - 1]);
         }
         vTaskDelay(pdMS_TO_TICKS(150));
         Max7219_Clear_Display();
         for (int i = 1; i < 9; i++)
         {
            /* code */
            Max7219_Write_Register(i, point[i - 1]);
         }
         vTaskDelay(pdMS_TO_TICKS(150));
      }
   }
}

uint8_t check_mov_y(int pos_x)
{
   int mov_y_axis = pos_x;
   uint8_t point_2x2;
   if (mov_y_axis > 1020)
   {
      point_2x2 = 0b11000000;
   }
   else if (mov_y_axis < 20)
   {
      point_2x2 = 0b000000011;
   }
   else
   {
      point_2x2 = 0b00011000;
   }
   return point_2x2;
}

uint8_t check_mov_x(int pos_y)
{
   uint8_t point_center;
   int mov_x_axis = pos_y;
   if (mov_x_axis > 1020)
   {
      point_center = 8;
   }
   else if (mov_x_axis < 20)
   {
      point_center = 2;
   }
   else
   {
      point_center = 5;
   }
   return point_center;
}

void User_Adc_Init(void)
{
   // Resolución del ADC a 10bits: 0 - 1024
   adc1_config_width(ADC_WIDTH_BIT_10);
   // Configuramos los canales a 11dB de atenuación: 3.3v
   adc1_config_channel_atten(USER_ADC_VRX, ADC_ATTEN_DB_11);
   adc1_config_channel_atten(USER_ADC_VRY, ADC_ATTEN_DB_11);
}

static void Max7219_Init(void)
{
   _max7219_func_config_t max7219_conf;
   max7219_conf.decode = NO_DECODE_FOR_DIGITS_7_0;
   max7219_conf.shutdown = NORMAL_OPERATION_SHUTDOWN;
   max7219_conf.intensity = DUTY_CICLE_1_32;
   max7219_conf.scan_limit = DISPLAY_DIG_0_7;
   max7219_conf.display_test = NORMAL_OPERATION_DISPLAY_TEST;
   Max7219_Config_Register(&max7219_conf);
}

static void Max7219_Config_Register(_max7219_func_config_t *config)
{
   Max7219_Write_Register(REGISTER_SHUTDOWN, config->shutdown);
   Max7219_Write_Register(REGISTER_DECODE_MODE, config->decode);
   Max7219_Write_Register(REGISTER_DISPLAY_TEST, config->display_test);
   Max7219_Write_Register(REGISTER_INTENSITY, config->intensity);
   Max7219_Write_Register(REGISTER_SCAN_LIMIT, config->scan_limit);
   Max7219_Clear_Display();
}

static void Max7219_Clear_Display(void)
{
   uint8_t count = 0;
   for (count = 0; count < 8; count++)
   {
      /* code */
      Max7219_Write_Register(count + 1, 0x00);
   }
}

static void Max7219_Set_Col(uint8_t col)
{
   uint8_t count = 0;
   for (count = 0; count < 8; count++)
   {
      /* code */
      Max7219_Write_Register(count + 1, 0x01 << col);
   }
}

static void Max7219_Set_Row(uint8_t row)
{
   Max7219_Write_Register(row + 1, 0x55);
}

static void Max7219_Write_Register(uint8_t registro, uint8_t dato)
{
   uint8_t data_to_transmit[2] = {registro, dato};
   spi_transaction_t transmit = {
       .tx_buffer = data_to_transmit,
       .length = 8 * sizeof(data_to_transmit)};
   spi_device_polling_transmit(spi, &transmit);
}

void User_Spi_Init(void)
{
   spi_bus_config_t user_spi =
       {
           .sclk_io_num = USER_SPI_CLK,
           .miso_io_num = USER_SPI_PIN_NO_USED,
           .mosi_io_num = USER_SPI_MOSI,
           .quadhd_io_num = USER_SPI_PIN_NO_USED,
           .quadwp_io_num = USER_SPI_PIN_NO_USED,
           .max_transfer_sz = SIZE_TO_SHARE_SPI};
   spi_bus_initialize(MAX7219E_HOST, &user_spi, SPI_DMA_CH_AUTO);

   spi_device_interface_config_t max7219_config =
       {
           .clock_speed_hz = 1000000, // Velocidad a 10Mhz
           .mode = 0,
           .spics_io_num = USER_SPI_CS,
           .flags = SPI_DEVICE_HALFDUPLEX,
           .queue_size = 1,
           .pre_cb = NULL,
           .post_cb = NULL};
   spi_bus_add_device(MAX7219E_HOST, &max7219_config, &spi);
}

void User_Gpio_Init(void)
{
   gpio_config_t but_user =
       {
           .intr_type = GPIO_INTR_DISABLE,
           .mode = GPIO_MODE_INPUT,
           .pull_down_en = GPIO_PULLDOWN_DISABLE,
           .pull_up_en = GPIO_PULLUP_ENABLE,
           .pin_bit_mask = (1ULL << USER_BUT_JOY)};
   gpio_config(&but_user);
}