/**
 * Planning out how we want to organize this...
 * - Each functionality is its own RTOS task?
 *  - xTaskCreatePinnedToCore() or xTaskCreateStaticPinnedToCore() w/ tskNO_AFFINITY
 * - That allows us to continue to listen on MQTT for flights while handling other events 
 */

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "wifi_sta.h"
#include "mqtt_wrapper.h"

// Logger tag
static const char *TAG = "app_main";

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

    esp_err_t esp_ret;

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

    esp_netif_init();
    esp_event_loop_create_default();

    esp_ret = wifi_sta_init(network_event_group);

    esp_ret = mqtt_start(mqtt_event_group);

    while (1) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
