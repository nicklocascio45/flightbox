#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define MQTT_CONNECTED_BIT			BIT0
#define MQTT_SUBSCRIBED_BIT			BIT1

/**
 * @brief Set up resources for MQTT client
 * 
 * This function takes care of all the initial setup for the MQTT
 * client and starts the client. An event handler is registered 
 * here so that future MQTT events and messages get processed
 * 
 * @param mqtt_event_group			Event group for MQTT events
 * @param lighting_event_group 		Event group to trigger lights based on message data
 * 
 * @return	ESP_OK: success
 * 			
 */
esp_err_t mqtt_start(EventGroupHandle_t mqtt_event_group, EventGroupHandle_t lighting_event_group, QueueHandle_t flight_queue);

#endif // MQTT_WRAPPER_H
