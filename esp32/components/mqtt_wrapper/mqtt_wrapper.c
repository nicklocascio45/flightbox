/**
 * 
 */

#include <string.h>
#include "esp_log.h"

#include "mqtt_client.h"
#include "cJSON.h"

#include "mqtt_wrapper.h"
#include "lighting.h"
#include "shared.h"

// Logger tag
static const char *TAG = "mqtt_wrapper";

// Static global variables
static EventGroupHandle_t s_mqtt_event_group = NULL;
static EventGroupHandle_t s_lighting_event_group = NULL;
static QueueHandle_t s_flight_queue = NULL;
static esp_mqtt_client_handle_t s_mqtt_client = NULL;

/****************************************************
 * Private function prototypes
 ****************************************************/

static void handle_flight(esp_mqtt_event_t *event);
static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/****************************************************
 * Private function definitions
 ****************************************************/

/**
 * @brief Function with all flight-specific handling logic
 * 
 * Checks the type of aircraft and sets appropriate lighting
 * bits. Then proceeds to extract all JSON fields and publishes
 * a flight handle to the shared queue for consumption by display
 * 
 * @param event     The MQTT event data with flight info
 */
static void handle_flight(esp_mqtt_event_t *event)
{
    // NOTE: IF PREVIOUS LIGHT IS ON, WE CAN UPDATE SCREEN BEFORE LIGHT IS OFF
    cJSON *flight_json = cJSON_ParseWithLength(event->data, event->data_len);
    cJSON *widebody = cJSON_GetObjectItem(flight_json, "widebody");

    if (widebody->type == cJSON_True) {
        ESP_LOGI(TAG, "Received message for widebody plane");
        xEventGroupSetBits(s_lighting_event_group, WIDEBODY_BIT);
    } else {
        ESP_LOGI(TAG, "Received message for standard plane");
        xEventGroupSetBits(s_lighting_event_group, STANDARD_BIT);
    }

    char *callsign = cJSON_GetObjectItem(flight_json, "callsign")->valuestring;
    char *operator = cJSON_GetObjectItem(flight_json, "operator")->valuestring;
    char *aircraft_type = cJSON_GetObjectItem(flight_json, "aircraft_type")->valuestring;
    char *origin = cJSON_GetObjectItem(flight_json, "origin")->valuestring;
    char *destination = cJSON_GetObjectItem(flight_json, "destination")->valuestring;
    char *area = cJSON_GetObjectItem(flight_json, "area")->valuestring;
    bool widebody_val = true ? widebody->type == cJSON_True : false;

    flight_t flight;
    strcpy(flight.callsign, callsign);
    strcpy(flight.operator, operator);
    strcpy(flight.aircraft_type, aircraft_type);
    strcpy(flight.origin, origin);
    strcpy(flight.destination, destination);
    strcpy(flight.area, area);
    flight.widebody = widebody_val;

    xQueueSend(s_flight_queue, &flight, portMAX_DELAY);
    cJSON_Delete(flight_json);
}

/**
 * @brief 
 * 
 * @param arg 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */
static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
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
            xEventGroupSetBits(s_mqtt_event_group, MQTT_SUBSCRIBED_BIT);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            break;

        case MQTT_EVENT_PUBLISHED:
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Received data!");
            handle_flight(event);
            break;

        case MQTT_EVENT_ERROR:
            break;

        default:
            ESP_LOGI(TAG, "Unhandled MQTT event: %li", event_id);
            break;
    }
}

/****************************************************
 * Public function definitions
 ****************************************************/

esp_err_t mqtt_start(EventGroupHandle_t mqtt_event_group, EventGroupHandle_t lighting_event_group, QueueHandle_t flight_queue)
{
    esp_err_t esp_ret;

    ESP_LOGI(TAG, "Starting MQTT client...");

    // Save event group handles and queue
    if (mqtt_event_group != NULL && lighting_event_group != NULL && flight_queue != NULL) {
        s_mqtt_event_group = mqtt_event_group;
        s_lighting_event_group = lighting_event_group;
        s_flight_queue = flight_queue;
    } else {
        ESP_LOGE(TAG, "One of the required params is NULL");
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
