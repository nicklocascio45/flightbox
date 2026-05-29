/**
 * 
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "mqtt_client.h"

#include "mqtt_wrapper.h"

// Logger tag
static const char *TAG = "mqtt_wrapper";

// Static global variables
static EventGroupHandle_t s_mqtt_event_group = NULL;
static esp_mqtt_client_handle_t s_mqtt_client = NULL;

static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    esp_mqtt_event_t *event = (esp_mqtt_event_t *)event_data;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected to MQTT broker");
            xEventGroupSetBits(s_mqtt_event_group, MQTT_CONNECTED_BIT);

            esp_mqtt_client_subscribe(s_mqtt_client, "/flightbox/flights/visible", 2);
            break;

        case MQTT_EVENT_DISCONNECTED:
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "Subscribed to topic");
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            break;

        case MQTT_EVENT_PUBLISHED:
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Received data!");
            ESP_LOGI(TAG, "  %.*s", event->data_len, event->data);
            break;

        case MQTT_EVENT_ERROR:
            break;

        default:
            ESP_LOGI(TAG, "Unhandled MQTT event: %li", event_id);
            break;
    }
}

esp_err_t mqtt_start(EventGroupHandle_t event_group) {
    esp_err_t esp_ret;

    ESP_LOGI(TAG, "Starting MQTT client...");

    // Save event group handle
    if (event_group != NULL) {
        s_mqtt_event_group = event_group;
    } else {
        ESP_LOGE(TAG, "Event group handle is NULL");
        return ESP_FAIL;
    }

    // Configure MQTT client
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = CONFIG_MQTT_BROKER_HOSTNAME,
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
        .broker.address.port = 1883,
        .credentials.username = CONFIG_MQTT_USERNAME,
        .credentials.authentication.password = CONFIG_MQTT_PASSWORD,
    };

    // Initialize MQTT client
    s_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    // Register MQTT event handler
    esp_mqtt_client_register_event(s_mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    // Start MQTT client
    esp_mqtt_client_start(s_mqtt_client);

    return ESP_OK;
}
