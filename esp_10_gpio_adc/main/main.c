/*
    Inclusiones
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <driver/dac.h>

/*
    Principal
*/

void app_main(void)
{
    // Elegimos la resolución del adc
    adc1_config_width(ADC_WIDTH_BIT_9);
    // Configuramos la atenuación
    adc1_config_channel_atten(ADC_CHANNEL_0, ADC_ATTEN_DB_11);

    // Configuramos el led por DAC
    dac_output_enable(DAC_CHANNEL_1); // Canal
    dac_output_voltage(DAC_CHANNEL_1, 0);

    // Hacemos las lecturas
    while (true)
    {
        /* code */
        int adc_raw_val = adc1_get_raw(ADC_CHANNEL_0);
        printf("Valor leido: %d\n\r", adc_raw_val);
        uint8_t dac_value = adc_raw_val / 2;
        dac_output_voltage(DAC_CHANNEL_1, dac_value);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
