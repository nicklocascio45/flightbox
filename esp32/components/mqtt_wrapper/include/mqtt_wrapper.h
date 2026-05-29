#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define MQTT_CONNECTED_BIT			BIT0

/**
 * @brief 
 * @param event_group 
 * @return 
 */
esp_err_t mqtt_start(EventGroupHandle_t event_group);

#endif // MQTT_WRAPPER_H
