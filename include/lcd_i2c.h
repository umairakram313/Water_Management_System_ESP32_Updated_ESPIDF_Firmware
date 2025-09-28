
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"


//initialize the LCD display
//Pin mappings from PCF8574 to LCD
#define RS (1 << 0)   // P0
#define RW (1 << 1)   // P1
#define EN (1 << 2)   // P2
#define BL (1 << 3)   // P3

#define D4 (1 << 4)
#define D5 (1 << 5)
#define D6 (1 << 6)
#define D7 (1 << 7)

// I2C master configuration
i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = 0,
    .scl_io_num = 22,
    .sda_io_num = 21,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};

i2c_master_bus_handle_t bus_handle;

i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x27,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t dev_handle;


void pcf8574write(uint8_t buffer);
void lcd_send_nibble(uint8_t nibble, uint8_t mode);
void lcd_send_byte(uint8_t byte, uint8_t mode);
void lcd_send_command(uint8_t cmd);
void lcd_write_char(char c);
void lcdInit();
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_write_string(char *string);
void lcd_clear();