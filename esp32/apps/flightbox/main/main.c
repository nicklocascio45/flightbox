#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "wifi_sta.h"
#include "mqtt_wrapper.h"
#include "lighting.h"
#include "display.h"

// Logger tag
static const char *TAG = "app_main";

// Queue
QueueHandle_t flight_queue;

typedef struct
{
    char callsign[32];
    char operator[32];
    char aircraft_type[32];
    char origin[64];
    char destination[64];
    bool widebody;
} flight_t;

void app_main(void)
{
    // Networking event resources
    EventGroupHandle_t network_event_group;
    EventBits_t network_event_bits;
    network_event_group = xEventGroupCreate();

    // MQTT event resources
    EventGroupHandle_t mqtt_event_group;
    EventBits_t mqtt_event_bits;
    mqtt_event_group = xEventGroupCreate();

    // Lighting event resources
    EventGroupHandle_t lighting_event_group;
    lighting_event_group = xEventGroupCreate();

    // Set up shared queue
    flight_queue = xQueueCreate(5, sizeof(flight_t));
    if (flight_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create flight queue");
        abort();
    }

    esp_err_t esp_ret;

    // Set up Nonvolatile Storage (NVS) - ESP32 WiFi driver uses NVS to store settings
    // Erase if out of space or new version
    esp_ret = nvs_flash_init();
    if (esp_ret == ESP_ERR_NVS_NO_FREE_PAGES || esp_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        if (nvs_flash_erase() != ESP_OK) {
            ESP_LOGE(TAG, "Failed to erase NVS partition");
            abort();
        }
        esp_ret = nvs_flash_init();
    }
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS (%d)", esp_ret);
        abort();
    }

    // Initialize network interface (can only be called once per app)
    // Must be called prior to network driver setup
    esp_netif_init();

    // Create default event loop, must be running prior to network driver setup
    esp_event_loop_create_default();

    // TODO: WiFi setup abstracted to a network wrapper
    // Initialize WiFi driver and ensure necessary bits get set
    esp_ret = wifi_sta_init(network_event_group);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "WiFi init process failed");
        abort();
    }

    EventBits_t wifi_bits = xEventGroupWaitBits(network_event_group,
                                                WIFI_CONNECTED_BIT | IP_OBTAINED_BIT,
                                                pdFALSE,
                                                pdTRUE,
                                                pdMS_TO_TICKS(10000));

    if ((wifi_bits & ( WIFI_CONNECTED_BIT | IP_OBTAINED_BIT )) == (WIFI_CONNECTED_BIT | IP_OBTAINED_BIT)) {
        ESP_LOGI(TAG, "WiFi is all good to go");
    } else if ((wifi_bits & WIFI_CONNECTED_BIT) != 0) {
        ESP_LOGE(TAG, "Only connected bit set, abort");
        abort();
    } else if ((wifi_bits & IP_OBTAINED_BIT) != 0) {
        ESP_LOGE(TAG, "Only IP obtained bit set, abort");
        abort();
    } else {
        ESP_LOGE(TAG, "Neither WiFi bit set, abort");
        abort();
    }

    // Start MQTT client and ensure necessary bits get set
    esp_ret = mqtt_start(mqtt_event_group, lighting_event_group, flight_queue);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "MQTT start process failed");
        abort();
    }

    EventBits_t mqtt_bits = xEventGroupWaitBits(mqtt_event_group,
                                                MQTT_CONNECTED_BIT | MQTT_SUBSCRIBED_BIT,
                                                pdFALSE,
                                                pdTRUE,
                                                pdMS_TO_TICKS(10000));

    if ((mqtt_bits & (MQTT_CONNECTED_BIT | MQTT_SUBSCRIBED_BIT)) == (MQTT_CONNECTED_BIT | MQTT_SUBSCRIBED_BIT)) {
        ESP_LOGI(TAG, "MQTT is all good to go");
    } else if ((mqtt_bits & MQTT_CONNECTED_BIT) != 0) {
        ESP_LOGE(TAG, "Only connected bit set, abort");
        abort();
    } else if ((mqtt_bits & MQTT_SUBSCRIBED_BIT) != 0) {
        ESP_LOGE(TAG, "Only subscribed bit set, abort");
        abort();
    } else {
        ESP_LOGE(TAG, "Neither MQTT bit set, abort");
        abort();
    }

    // Create the lighting task
    // TaskHandle_t lighting_handle;
    // xTaskCreate(lighting_task,
    //             "lighting_task",
    //             4096,
    //             (void *)lighting_event_group,
    //             10,
    //             &lighting_handle);

    esp_ret = display_init();
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Display init process failed");
        abort();
    }

    TaskHandle_t display_handle;
    xTaskCreate(display_task,
                "display_task",
                4096,
                (void *)flight_queue,
                5,
                &display_handle);

    // Infinite loop, program is now all tasks and event handlers
    while (1) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
