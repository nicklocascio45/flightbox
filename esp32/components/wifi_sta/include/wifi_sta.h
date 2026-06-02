#ifndef WIFI_STA_H
#define WIFI_STA_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT		BIT0
#define IP_OBTAINED_BIT			BIT1

/**
 * @brief Perform all wifi initialization operations
 * 
 * This function initializes, configures, and sets up the core WiFi
 * driver for the system. It also registers event handlers for WiFi
 * and IP events for all future event handling
 * 
 * @param event_group 	The event group for passing WiFi events between components/tasks
 * 
 * @return	ESP_OK: success
 * 			ESP_FAIL: problem occurred during setup process
 */
esp_err_t wifi_sta_init(EventGroupHandle_t event_group);

/**
 * @brief 
 * @param  
 * @return 
 */
esp_err_t wifi_sta_stop(void);

/**
 * @brief 
 * @param  
 * @return 
 */
esp_err_t wifi_sta_reconnect(void);

#endif // WIFI_STA_H
