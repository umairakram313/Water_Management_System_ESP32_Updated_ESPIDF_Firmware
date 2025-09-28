#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_mac.h"

#include "driver/i2c_master.h"
#include "lcd_i2c.h"

#define BLYNK_TEMPLATE_ID "TMPL6RFS_EsLd"
#define BLYNK_TEMPLATE_NAME "Water Level Monitoring"
#define BLYNK_AUTH_TOKEN "wu5HrRpRE07-dLJErpFe1ZGznUm6iCl7"

#define EXAMPLE_I2C_HW_ADDR 0x27 // Common I2C address for LCD backpacks
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ 400000 // 400kHz is typical for I2C LCDs
#define EXAMPLE_LCD_CMD_BITS 8 // Usually 8 bits for LCD commands

#define LED1 2
#define LED2 4
#define LED3 5
#define LED4 18
#define trig 12
#define echo 13
#define relay 14
#define level 39
#define buzzer 32

//Water Level Thresholds and initializing other variables
const int MaxLevel = 10; 
int Level1 = (MaxLevel * 75) / 100; 
int Level2 = (MaxLevel * 65) / 100;
int Level3 = (MaxLevel * 55) / 100;
int Level4 = (MaxLevel * 35) / 100;
int val = 0;
int lo = 0;
long t;
int distance;
int blynkDistance;


// Enter your Auth token
char auth[] = "wu5HrRpRE07-dLJErpFe1ZGznUm6iCl7";

//Enter your WIFI SSID and password
char ssid[] = "Oppo X";
char pass[] = "oppo5298";

void app_main(void)
{

}
