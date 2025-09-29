idf_component_register(
    SRCS "blynk_iot.c" "lcd_i2c.c"
    INCLUDE_DIRS "." "../include"
    REQUIRES driver freertos
)
