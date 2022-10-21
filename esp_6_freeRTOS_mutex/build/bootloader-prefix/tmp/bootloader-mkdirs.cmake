# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.0/components/bootloader/subproject"
  "D:/Escritorio/esp32_codigos_clase/esp_6_freeRTOS_mutex/build/bootloader"
  "D:/Escritorio/esp32_codigos_clase/esp_6_freeRTOS_mutex/build/bootloader-prefix"
  "D:/Escritorio/esp32_codigos_clase/esp_6_freeRTOS_mutex/build/bootloader-prefix/tmp"
  "D:/Escritorio/esp32_codigos_clase/esp_6_freeRTOS_mutex/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Escritorio/esp32_codigos_clase/esp_6_freeRTOS_mutex/build/bootloader-prefix/src"
  "D:/Escritorio/esp32_codigos_clase/esp_6_freeRTOS_mutex/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Escritorio/esp32_codigos_clase/esp_6_freeRTOS_mutex/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Escritorio/esp32_codigos_clase/esp_6_freeRTOS_mutex/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
