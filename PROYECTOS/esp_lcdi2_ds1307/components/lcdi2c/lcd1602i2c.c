#include "lcd1602i2c.h"
 
/* Función para encender o apagar el backlight del LCD */

void Lcd_I2c_Backlight_Status (_back_light_status_t status)
{
    uint8_t com = (0x08 & ~(status << 3));
    if (com & 0x08)
    {
        general_back_light_var = 1;
    }
    else
    {
        general_back_light_var = 0;
    }
    
    i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &com , sizeof(com), pdMS_TO_TICKS(1000));
}

/* Función para inciar el LCD */

void Lcd_I2c_Send_Byte_With_Toggle_Enable (char data, _mode_lcd_control_t modo)
{
    /* Construimos los Nibles a mandar*/
    uint8_t higher_bits = (data & 0xF0); // Los 4 bits MSB
    uint8_t lower_bits = (data & 0x0F) << 4; // Lols 4 LSB

    // Cargamos el modo al cual se envian los nibles

    higher_bits |= ((modo & 0x01) << 0); // El & 0x01 es una contigencia provicinal
    lower_bits |= ((modo & 0x01) << 0);

    // Para esta prueba, dejaremos el rw en 0 siempre por lo tanto

    uint8_t rw_state = 0;
    higher_bits |= ((rw_state & 0x01) << 1);
    lower_bits |= ((rw_state & 0x01) << 1);

    // Depende de la configuración del backlight lo tomamos como referencia
    higher_bits |= ((general_back_light_var & 0x01) << 3);
    lower_bits |= ((general_back_light_var & 0x01) << 3);

    // Ahora mandamos los datos por el i2c con los bytes construidos
    
    // Mandamos los 4 MSB

    i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &higher_bits, sizeof(higher_bits), pdMS_TO_TICKS(10));
    higher_bits |= (1 << 2);
    i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &higher_bits, sizeof(higher_bits), pdMS_TO_TICKS(10));
    vTaskDelay(pdMS_TO_TICKS(3));
    higher_bits &= ~(1 << 2);
    i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &higher_bits, sizeof(higher_bits), pdMS_TO_TICKS(10));

    // Mandamos los 4 LAB

    i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &lower_bits, sizeof(lower_bits), pdMS_TO_TICKS(10));
    lower_bits |= (1 << 2);
    i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &lower_bits, sizeof(lower_bits), pdMS_TO_TICKS(10));
    vTaskDelay(pdMS_TO_TICKS(3));
    lower_bits &= ~(1 << 2);
    i2c_master_write_to_device(I2C_NUM_0, LCDI2C_ADDRESS, &lower_bits, sizeof(lower_bits), pdMS_TO_TICKS(10));

    /* Limpiamos las variables */

    higher_bits = 0x00;
    lower_bits = 0x00;
}

/* Funcion para inicializar el LCD */

void Lcd_I2c_Init (void)
{
    Lcd_I2c_Send_Byte_With_Toggle_Enable(LCDI2C_CLEAR, MODO_COMANDO);
    Lcd_I2c_Send_Byte_With_Toggle_Enable(LCDI2C_HOME, MODO_COMANDO);
    Lcd_I2c_Send_Byte_With_Toggle_Enable(LCDI2C_ENTRY_MODE, MODO_COMANDO);
    Lcd_I2c_Send_Byte_With_Toggle_Enable(LCDI2C_DISPLAY_ON, MODO_COMANDO);
    Lcd_I2c_Send_Byte_With_Toggle_Enable(LCDI2C_FUNCTION_SET, MODO_COMANDO);
    Lcd_I2c_Send_Byte_With_Toggle_Enable(LCDI2C_SET_CURSOR, MODO_COMANDO);
}