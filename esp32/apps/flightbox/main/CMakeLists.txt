idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS ""
    REQUIRES esp_netif nvs_flash wifi_sta mqtt_wrapper
)
