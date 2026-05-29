#ifndef WIFI_STA_H
#define WIFI_STA_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

esp_err_t wifi_sta_init(EventGroupHandle_t event_group);
esp_err_t wifi_sta_stop(void);
esp_err_t wifi_sta_reconnect(void);

#endif // WIFI_STA_H
