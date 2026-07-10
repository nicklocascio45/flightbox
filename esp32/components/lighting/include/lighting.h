/**
 * 
 */

#define GREEN_LED_PIN       37
#define RED_LED_PIN         38

#define STANDARD_BIT			BIT0
#define WIDEBODY_BIT			BIT1

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
