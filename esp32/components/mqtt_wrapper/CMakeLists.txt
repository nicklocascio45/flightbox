set(srcs)
set(include_dirs "include")

list(APPEND srcs "mqtt_wrapper.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_REQUIRES mqtt cjson lighting shared)
