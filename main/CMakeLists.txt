idf_component_register(SRCS "water-management.c" 
                    INCLUDE_DIRS "." "../include"
                    REQUIRES esp_driver_i2c 
                    src esp_wifi nvs_flash 
                    esp_netif esp_http_client 
                    esp_timer esp_adc)
