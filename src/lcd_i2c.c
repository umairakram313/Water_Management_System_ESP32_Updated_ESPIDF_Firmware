#include "lcd_i2c.h"

uint8_t BL = 0;

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

void pcf8574write(uint8_t buffer)
{
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &buffer, 1, -1));
}

void lcd_send_nibble(uint8_t nibble, uint8_t mode)
{
    uint8_t data=0;
    if(mode)
        data|=RS;
    data|=BL;
    data |= (nibble&0x0F)<<4;
    //Pulse the Enable pin
    pcf8574write(data|EN);
    //Wait for a short period
    vTaskDelay(1/portTICK_PERIOD_MS);
    //Bring the Enable pin low
    pcf8574write(data&~EN);
}

void lcd_send_byte(uint8_t byte, uint8_t mode)
{
    lcd_send_nibble(byte>>4, mode);
    lcd_send_nibble(byte&0x0F, mode);
}

void lcd_send_command(uint8_t cmd)
{
    lcd_send_byte(cmd, 0);
}

void lcd_write_char(char c)
{
    lcd_send_byte(c, 1);
}

void lcdInit()
{
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    lcd_send_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    lcd_send_nibble(0x02, 0);
    //lcd is now in 4-bit mode
    lcd_send_command(0x28); //Function set: 4-bit/2-line
    lcd_send_command(0x08); //Display off
    lcd_send_command(0x01); //Display clear
    vTaskDelay(pdMS_TO_TICKS(2));
    lcd_send_command(0x06); //Entry mode set
    lcd_send_command(0x0C); //Display on, cursor off, blink off
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    //this func is for size 16x2
    uint8_t row_col_data=0;
    if(row==2)
        row_col_data|=0x40;
    col&=0x0F;
    row_col_data|=col;
    uint8_t cmd = 0x80 | row_col_data;
    lcd_send_command(cmd);
}

void lcd_write_string(char *string)
{
    while(*string)
    {
        lcd_write_char(*string++);
    }
}

void lcd_clear()
{
    lcd_send_command(0x01);
}

void lcd_backlight_on()
{
    BL=(1<<3);
    lcd_send_command(0x00);

}

void lcd_backlight_off()
{
    BL=0;
    lcd_send_command(0x00);
}