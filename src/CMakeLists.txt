idf_component_register(
    SRCS "blynk_iot.c" "lcd_i2c.c"
    INCLUDE_DIRS "." "../include"
    REQUIRES driver freertos esp_wifi nvs_flash esp_netif esp_http_client   
)
