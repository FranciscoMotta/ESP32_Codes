/* * Data converted from D:/Escritorio/esp32_codigos_clase/9_Storage_ESP32/main/files/texto.txt
 * (null byte appended)
 */
.data
.section .rodata.embedded

.global texto_txt
texto_txt:

.global _binary_texto_txt_start
_binary_texto_txt_start: /* for objcopy compatibility */
.byte 0x54, 0x65, 0x78, 0x74, 0x6f, 0x20, 0x73, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x00

.global _binary_texto_txt_end
_binary_texto_txt_end: /* for objcopy compatibility */


.global texto_txt_length
texto_txt_length: /* not including null byte */
.word 12
