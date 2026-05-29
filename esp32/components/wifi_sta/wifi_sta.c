/**
 * 
 */

#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

// Logger tag
static const char *TAG = "wifi_sta";

// Static global variables
static EventGroupHandle_t s_wifi_event_group = NULL;

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    esp_err_t esp_ret;

    switch (event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WiFi started, connecting...");
            esp_ret = esp_wifi_connect();
            if (esp_ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to start connection process");
            } else {
                ESP_LOGI(TAG, "Started connection process");
            }
            break;

        case WIFI_EVENT_STA_STOP:
            break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "WiFi connected!!");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            break;

        default:
            ESP_LOGI(TAG, "Unhandled WiFi event: %li", event_id);
            break;
    }
}

static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP:
            break;
        case IP_EVENT_STA_LOST_IP:
            break;
        default:
            ESP_LOGI(TAG, "Unhandled IP event: %li", event_id);
            break;
    }
}

esp_err_t wifi_sta_init(EventGroupHandle_t event_group)
{
    esp_err_t esp_ret;

    ESP_LOGI(TAG, "Initializing WiFi in station mode...");

    // Save event group handle
    if (event_group != NULL) {
        s_wifi_event_group = event_group;
    } else {
        ESP_LOGE(TAG, "Event group handle is NULL");
        return ESP_FAIL;
    }

    // Create default WiFi STA network interface
    esp_netif_t *wifi_netif;
    wifi_netif = esp_netif_create_default_wifi_sta();

    // Initialize WiFi driver
    wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_ret = esp_wifi_init(&wifi_init_cfg);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi driver");
        return ESP_FAIL;
    }

    // Register event handlers
    esp_ret = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register WiFi event handler");
        return ESP_FAIL;
    }

    esp_ret = esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register IP event handler");
        return ESP_FAIL;
    }

    // Configure the WiFi driver
    esp_ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi mode");
        return ESP_FAIL;
    }

    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = CONFIG_WIFI_STA_SSID,
            .password = CONFIG_WIFI_STA_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK, // TODO: add to Kconfig?
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,         // TODO: add to Kconfig?
            .sae_h2e_identifier = "",                 // TODO: add to Kconfig?
        },
    };
    esp_ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi configuration");
        return ESP_FAIL;
    }

    // Start the WiFi driver
    esp_ret = esp_wifi_start();
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WiFi driver");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t wifi_sta_stop()
{
    return ESP_OK;
}

esp_err_t wifi_sta_reconnect()
{
    return ESP_OK;
}
