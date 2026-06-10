#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "lvgl.h"
#include "esp_lcd_ili9341.h"

#define LCD_SPI_HOST	SPI2_HOST
#define SCL_PIN			17
#define SDA_PIN			16
#define RES_PIN			8
#define DC_PIN			9
#define CS_PIN			10
#define BLK_PIN			6
#define RST_PIN			8

#define LCD_HOR_RES		240
#define LCD_VER_RES		320

// Logger tag
static const char *TAG = "display";

static void lvgl_flush_callback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
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
		.max_transfer_sz = LCD_HOR_RES * 80 * sizeof(uint16_t),
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
		.pclk_hz = 20 * 1000 * 1000, 	// why?
		.lcd_cmd_bits = 8, 				// why?
		.lcd_param_bits = 8,			// why?
		.spi_mode = 0,					// why?
		.trans_queue_depth = 10,		// why?
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
		.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
		.bits_per_pixel = 16,
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
	// Add callback that can flush content from buffer when it has been rendered
	lv_display_set_flush_cb(display, lvgl_flush_callback);
	ESP_LOGI(TAG, "Display has been configured");

	return ESP_OK;
}
