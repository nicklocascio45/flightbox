#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "lvgl.h"
#include "esp_lcd_ili9341.h"

#define LCD_SPI_HOST			SPI2_HOST
#define SCL_PIN					17
#define SDA_PIN					16
#define RES_PIN					8
#define DC_PIN					9
#define CS_PIN					10
#define BLK_PIN					6
#define RST_PIN					8

#define LCD_HOR_RES				240
#define LCD_VER_RES				320
#define LCD_PIXEL_CLOCK_HZ		20 * 1000 * 1000 	// how fast ESP32 can send data to update the LEDs, 20MHz here
#define LCD_CMD_BITS			8 					// bit width of command recognized by LCD controller, ILI9341 uses 8 bits
#define LCD_PARAM_BITS			8 					// bit width of parameter recognized by LCD controller, ILI9341 uses 8 bits

#define LVGL_TICK_PERIOD_MS		2

// Lock to use during LVGL API calls from tasks bc the APIs are not thread safe
static _lock_t lvgl_api_lock;

// Logger tag
static const char *TAG = "display";

/****************************************************
 * Private function prototypes
 ****************************************************/

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_io_event_data_t *data, void *user_ctx);
static void lvgl_port_update_callback(lv_display_t *disp);
static void lvgl_flush_callback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void lvgl_tick(void *arg);
static void lvgl_timer_task(void *arg);

/****************************************************
 * Private function definitions
 ****************************************************/

/**
 * @brief Called by ESP display when color data is ready to be flushed to display
 *
 * Simply calls lv_display_flush_ready to inform LVGL that color data is ready
 *
 * @param io_handle
 * @param data
 * @param user_ctx
 *
 * @return
 */
static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_io_event_data_t *data, void *user_ctx)
{
	lv_display_t *disp = (lv_display_t *)user_ctx;
	lv_display_flush_ready(disp);
	return false;
}

/**
 * @brief Rotate display when screen rotated in LVGL, called when driver params are updated
 */
static void lvgl_port_update_callback(lv_display_t *disp)
{
	esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
	lv_display_rotation_t rotation = lv_display_get_rotation(disp);

	switch (rotation) {
		case LV_DISPLAY_ROTATION_0:
			// Rotate LCD display
			esp_lcd_panel_swap_xy(panel_handle, false);
			esp_lcd_panel_mirror(panel_handle, true, false);
			break;
		case LV_DISPLAY_ROTATION_90:
			// Rotate LCD display
			esp_lcd_panel_swap_xy(panel_handle, true);
			esp_lcd_panel_mirror(panel_handle, true, true);
			break;
		case LV_DISPLAY_ROTATION_180:
			// Rotate LCD display
			esp_lcd_panel_swap_xy(panel_handle, false);
			esp_lcd_panel_mirror(panel_handle, false, true);
			break;
		case LV_DISPLAY_ROTATION_270:
			// Rotate LCD display
			esp_lcd_panel_swap_xy(panel_handle, true);
			esp_lcd_panel_mirror(panel_handle, false, false);
			break;
	}
}

/**
 * @brief Responsible for flushing color data on the display area
 *
 * Calls lvgl_port_update_callback to update the display resolution
 * and then draws color data on the required display area
 *
 * @param disp
 * @param area
 * @param px_map
 */
static void lvgl_flush_callback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
	lvgl_port_update_callback(disp);
	esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
	int offsetx1 = area->x1;
	int offsetx2 = area->x2;
	int offsety1 = area->y1;
	int offsety2 = area->y2;
	// Swap RGB bytes order bc SPI is big-endian
	lv_draw_sw_rgb565_swap(px_map, (offsetx2 + 1 - offsetx1) * (offsety2 + 1 - offsety1));
	// Copy buffer content to specific area of display
	esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
}

/**
 * @brief Used to tell LVGL how many milliseconds have elapsed
 */
static void lvgl_tick(void *arg)
{
	lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

/**
 * @brief FreeRTOS task used to call the lvgl timer handler periodically
 */
static void lvgl_timer_task(void *arg)
{
	uint32_t time_till_next_ms = 0;
	uint32_t time_threshold_ms = 2000 / CONFIG_FREERTOS_HZ;
	while (1) {
		_lock_acquire(&lvgl_api_lock);
		time_till_next_ms = lv_timer_handler();
		_lock_release(&lvgl_api_lock);
		// in case of triggering a task watch dog time out
		time_till_next_ms = MAX(time_till_next_ms, time_threshold_ms);
		vTaskDelay(time_till_next_ms);
	}
}

/****************************************************
 * Public function definitions
 ****************************************************/

esp_err_t display_init(void)
{
	esp_err_t esp_ret;

	// Set up LCD backlight GPIO
	gpio_config_t blk_gpio_cfg = {
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = 1ULL << BLK_PIN,
	};
	esp_ret = gpio_config(&blk_gpio_cfg);
	if (esp_ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to set up backlight GPIO");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "Set up backlight GPIO");

	// Initialize SPI bus
	spi_bus_config_t bus_cfg = {
		.mosi_io_num = SDA_PIN,
		.sclk_io_num = SCL_PIN,
		.quadwp_io_num = -1, 										// not using QUADSPI
		.quadhd_io_num = -1, 										// not using QUADSPI
		.max_transfer_sz = LCD_HOR_RES * 80 * sizeof(uint16_t), 	// transfer 80 lines of pixels at most in one SPI transaction
	};
	esp_ret = spi_bus_initialize(LCD_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
	if (esp_ret != ESP_OK) {
		ESP_LOGE(TAG, "Error initializing SPI bus");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "SPI bus initialized");

	// Install panel IO
	esp_lcd_panel_io_handle_t io_handle = NULL;
	esp_lcd_panel_io_spi_config_t io_cfg = {
		.dc_gpio_num = DC_PIN,
		.cs_gpio_num = CS_PIN,
		.pclk_hz = LCD_PIXEL_CLOCK_HZ,
		.lcd_cmd_bits = LCD_CMD_BITS,
		.lcd_param_bits = LCD_PARAM_BITS,
		.spi_mode = 0,
		.trans_queue_depth = 10,
	};
	esp_ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_HOST, &io_cfg, &io_handle);
	if (esp_ret != ESP_OK) {
		ESP_LOGE(TAG, "Error installing panel IO");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "Installed panel IO");

	// Install panel driver
	esp_lcd_panel_handle_t panel_handle = NULL;
	esp_lcd_panel_dev_config_t panel_cfg = {
		.reset_gpio_num = RST_PIN,
		.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,		// ILI9341 uses BGR element order
		.bits_per_pixel = 16,							// RGB565 color data bit width requires 2 bytes per pixel
	};
	esp_ret = esp_lcd_new_panel_ili9341(io_handle, &panel_cfg, &panel_handle);
	if (esp_ret != ESP_OK) {
		ESP_LOGE(TAG, "Error installing panel driver");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "Installed panel driver");

	// Set up panel
	esp_lcd_panel_reset(panel_handle);
	esp_lcd_panel_init(panel_handle);
	esp_lcd_panel_mirror(panel_handle, true, false);
	esp_lcd_panel_disp_on_off(panel_handle, true);
	gpio_set_level(BLK_PIN, 1);
	ESP_LOGI(TAG, "Panel set up");

	// Initialize LVGL
	lv_init();

	// Create LVGL display
    lv_display_t *display = lv_display_create(LCD_HOR_RES, LCD_VER_RES);

	// Allocate buffers used by LVGL
	size_t draw_buffer_sz = LCD_HOR_RES * 20 * sizeof(lv_color16_t);
	void *buf1 = spi_bus_dma_memory_alloc(LCD_SPI_HOST, draw_buffer_sz, 0);
	void *buf2 = spi_bus_dma_memory_alloc(LCD_SPI_HOST, draw_buffer_sz, 0);
	if (buf1 == NULL || buf2 == NULL) {
		ESP_LOGE(TAG, "Failed to allocate buffers for LVGL");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "Buffers allocated for LVGL");

	// Initialize draw buffers
	lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
	
	// Associate panel handle to display
	lv_display_set_user_data(display, panel_handle);
	
	// Set color depth
	lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
	
	// Register buffer flush callback
	lv_display_set_flush_cb(display, lvgl_flush_callback);
	
	// Install LVGL tick timer
	const esp_timer_create_args_t lvgl_tick_timer_args = {
		.callback = &lvgl_tick,
		.name = "lvgl_tick",
	};
	esp_timer_handle_t lvgl_tick_timer = NULL;
	esp_ret = esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
	if (esp_ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to create LVGL tick timer");
		return ESP_FAIL;
	}
	esp_ret = esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000);
	if (esp_ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to start LVGL tick timer");
		return ESP_FAIL;
	}

	// Register flush ready notifier
	const esp_lcd_panel_io_callbacks_t cbs = {
		.on_color_trans_done = notify_lvgl_flush_ready,
	};
	esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, display);

	ESP_LOGI(TAG, "LVGL display has been fully configured");

	// Simple test display
	_lock_acquire(&lvgl_api_lock);
	lv_color_t bg = {
		.blue = 255,
		.green = 0,
		.red = 0,
	};
	lv_color_t text = {
		.blue = 0,
		.green = 255,
		.red = 0,
	};
	lv_obj_set_style_bg_color(lv_screen_active(), bg, LV_PART_MAIN);

	lv_obj_t *label = lv_label_create(lv_screen_active());
	lv_label_set_text(label, "Hello, world!");
	lv_obj_set_style_text_color(lv_screen_active(), text, LV_PART_MAIN);
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	_lock_release(&lvgl_api_lock);

	// Create LVGL timer task
	xTaskCreate(lvgl_timer_task, "LVGL_timer", 4 * 1024, NULL, 2, NULL);
	ESP_LOGI(TAG, "LVGL timer task created");

	return ESP_OK;
}
