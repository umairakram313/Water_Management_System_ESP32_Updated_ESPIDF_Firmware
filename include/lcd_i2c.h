
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"


//initialize the LCD display
//Pin mappings from PCF8574 to LCD
#define RS (1 << 0)   // P0
#define RW (1 << 1)   // P1
#define EN (1 << 2)   // P2
extern uint8_t BL;   // P3 (modifiable, backlight control)

#define D4 (1 << 4)
#define D5 (1 << 5)
#define D6 (1 << 6)
#define D7 (1 << 7)

// I2C master configuration
extern i2c_master_bus_config_t i2c_mst_config;
extern i2c_master_bus_handle_t bus_handle;
extern i2c_device_config_t dev_cfg;
extern i2c_master_dev_handle_t dev_handle;


void pcf8574write(uint8_t buffer);
void lcd_send_nibble(uint8_t nibble, uint8_t mode);
void lcd_send_byte(uint8_t byte, uint8_t mode);
void lcd_send_command(uint8_t cmd);
void lcd_write_char(char c);
void lcdInit();
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_write_string(char *string);
void lcd_clear();
void lcd_backlight_on();
void lcd_backlight_off();