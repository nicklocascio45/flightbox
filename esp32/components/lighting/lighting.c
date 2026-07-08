#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "lighting.h"

#define LIGHT_TIME_SEC		5

// Logger tag
static const char *TAG = "lighting";

/****************************************************
 * Private function prototypes
 ****************************************************/

void standard_light(void);
void widebody_light(void);
void widebody_alt_light(void);
void alert_light(void);

/****************************************************
 * Private function definitions
 ****************************************************/

// Lighting for a standard plane in main AOI
void standard_light(void)
{
    gpio_set_level(GREEN_LED_PIN, 1);
    vTaskDelay(LIGHT_TIME_SEC * 1000 / portTICK_PERIOD_MS);
    gpio_set_level(GREEN_LED_PIN, 0);
}

// Lighting for a widebody plane in main AOI
void widebody_light(void)
{
    gpio_set_level(RED_LED_PIN, 1);
    vTaskDelay(LIGHT_TIME_SEC * 1000 / portTICK_PERIOD_MS);
    gpio_set_level(RED_LED_PIN, 0);
}

// Lighting for widebody plane in alternate AOI
void widebody_alt_light(void)
{

}

// Lighting for emergency squawk, military aircraft, etc.
void alert_light(void)
{

}

/****************************************************
 * Public function definitions
 ****************************************************/

// Core lighting task target function
void lighting_task(void *pvParameters)
{
    EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;

    // Reset LED pins
    gpio_reset_pin(GREEN_LED_PIN);
    gpio_reset_pin(RED_LED_PIN);

    // Set LED GPIO directions
    gpio_set_direction(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(RED_LED_PIN, GPIO_MODE_OUTPUT);

    // Start all LEDs on low
    gpio_set_level(GREEN_LED_PIN, 0);
    gpio_set_level(RED_LED_PIN, 0);

	// Enter core event loop
    while (1) {
        EventBits_t light_bits = xEventGroupWaitBits(event_group,
                                                    STANDARD_BIT | WIDEBODY_BIT | WIDEBODY_ALT_BIT | WARNING_BIT,
                                                    pdTRUE,
                                                    pdFALSE,
                                                    pdMS_TO_TICKS(10000));

		// Check if any bits were set and kick off lighting appropriately
        if ((light_bits & STANDARD_BIT) != 0) {
            ESP_LOGI(TAG, "Standard plane light");
            standard_light();
        } else if ((light_bits & WIDEBODY_BIT) != 0) {
            ESP_LOGI(TAG, "Widebody plane light");
            widebody_light();
        } else if ((light_bits & WIDEBODY_ALT_BIT) != 0) {
            ESP_LOGI(TAG, "Widebody alt plane light");
        } else if ((light_bits & WARNING_BIT) != 0) {
            ESP_LOGI(TAG, "Warning light");
        }
    }
}
