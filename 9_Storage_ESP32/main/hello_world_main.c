/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/*
    INCLUSIONES
*/

#include <stdio.h>
#include "sdkconfig.h"

/*
    MACROS
*/

#define K_BYTE_LONG 1024

/*
    PRINCIPAL
*/

void app_main(void)
{
    extern const unsigned char indexHtml[] asm("_binary_index_html_start");
    printf("HTML = %s\n", indexHtml);

    extern const unsigned char sampleTxt[] asm("_binary_texto_txt_start");
    printf("Texto = %s\n", sampleTxt);

    extern const unsigned char imgStart[] asm("_binary_pinout_png_start");
    extern const unsigned char imgEnd[] asm("_binary_pinout_png_end");

    const unsigned int imgSize = (imgEnd - imgStart) / K_BYTE_LONG;
    printf("El tamano de la imagen es: %d en KB \n", imgSize);
}
