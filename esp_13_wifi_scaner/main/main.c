/*
   LIBRERIAS
*/

#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

/*
   MACROS
*/

#define MAX_APS 20
#define DEFAULT_SSID "TEST_SSID"
#define DEFAULT_PWD "TEST_PASS"

/*
   VARIABLES
*/

static esp_err_t event_handler(void *ctx, esp_event_base_t *event_base)
{
   return ESP_OK;
}

/*
   DECLARACIÓN DE FUNCIONES
*/

void User_Wifi_Init(void);
static char *getAuthModeName(wifi_auth_mode_t auth_mode);

/*
   PRINCIPAL
*/

void app_main(void)
{
   User_Wifi_Init();
   printf("Wifi - Scaner!\n");

   // Configuramos e iniciamos el Wifi
   wifi_scan_config_t wifi_config = {
      .ssid = DEFAULT_SSID, // ID
      .bssid = 0, // MAC
      .channel = 0, // Canal
      .show_hidden = true
   };
   ESP_ERROR_CHECK(esp_wifi_scan_start(&wifi_config, true));

   // Hacemos el scanneo
   uint16_t max_records = MAX_APS;
   wifi_ap_record_t wifi_scanned[MAX_APS];
   ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&max_records, wifi_scanned));
   printf("Found %d access points:\n", max_records);
   printf("\n");
   printf("               SSID              | Channel | RSSI |   Auth Mode \n");
   printf("----------------------------------------------------------------\n");
   for (int i = 0; i < max_records; i++)
      printf("%32s | %7d | %4d | %12s\n", (char *)wifi_scanned[i].ssid, wifi_scanned[i].primary, wifi_scanned[i].rssi, getAuthModeName(wifi_scanned[i].authmode));
   printf("----------------------------------------------------------------\n");
}

/*
   DEFINICION DE SUBFUNCIONES
*/

static char *getAuthModeName(wifi_auth_mode_t auth_mode)
{
   char *names[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA MPA2 PSK", "MAX"};
   return names[auth_mode];
}

void User_Wifi_Init(void)
{
   ESP_ERROR_CHECK(nvs_flash_init()); // Iniciamos el NVS (Non Volatile Storage)
   ESP_ERROR_CHECK(esp_netif_init()); // Este es el reemplazo del tcpip_adapter_init()
   ESP_ERROR_CHECK(esp_event_loop_create_default());
   wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();

   ESP_ERROR_CHECK(esp_wifi_init(&config));

   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   ESP_ERROR_CHECK(esp_wifi_start());
}