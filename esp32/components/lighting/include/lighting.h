/**
 * 
 */

#define GREEN_LED_PIN       8
#define RED_LED_PIN         9
#define YELLOW_LED_PIN      10

#define STANDARD_BIT			BIT0
#define WIDEBODY_BIT			BIT1
#define WIDEBODY_ALT_BIT		BIT2
#define WARNING_BIT				BIT3

/**
 * @brief The core driver for lighting functionality
 *
 * This function is the target of a FreeRTOS task creation. It starts
 * by performing all initial LED setup. It then enters an infinite loop
 * of waiting for event group bits. If it any point a bit gets set,
 * it checks which bit was set and runs the appropriate internal
 * lighting function
 *
 * @param pvParameters	The param passed to the task, in this case a lighting event group
 */
void lighting_task(void *pvParameters);
