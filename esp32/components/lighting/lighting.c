#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "lighting.h"

#define LIGHT_TIME_SEC		0.5
#define LIGHT_BLINK_COUNT   5

// Logger tag
static const char *TAG = "lighting";

/****************************************************
 * Private function prototypes
 ****************************************************/

static void blink_light(uint32_t light_pin);

/****************************************************
 * Private function definitions
 ****************************************************/

void blink_light(uint32_t light_pin)
{
    for (int i = 0; i < LIGHT_BLINK_COUNT; i++) {
        gpio_set_level(light_pin, 1);
        vTaskDelay(LIGHT_TIME_SEC * 1000 / portTICK_PERIOD_MS);
        gpio_set_level(light_pin, 0);
        vTaskDelay(LIGHT_TIME_SEC * 1000 / portTICK_PERIOD_MS);
    }
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
                                                    STANDARD_BIT | WIDEBODY_BIT,
                                                    pdTRUE,
                                                    pdFALSE,
                                                    pdMS_TO_TICKS(10000));

		// Check if any bits were set and kick off lighting appropriately
        if ((light_bits & STANDARD_BIT) != 0) {
            blink_light(GREEN_LED_PIN);
        } else if ((light_bits & WIDEBODY_BIT) != 0) {
            blink_light(RED_LED_PIN);
        }
    }
}
