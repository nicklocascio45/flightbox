set(srcs)
set(include_dirs "include")

list(APPEND srcs "lighting.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_REQUIRES esp_driver_gpio)
