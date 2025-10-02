#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_mac.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include <esp_timer.h>

#include "lcd_i2c.h"
#include "blynk_iot.h"

#define BLYNK_TEMPLATE_ID "TMPL6RFS_EsLd"
#define BLYNK_TEMPLATE_NAME "Water Level Monitoring"
#define BLYNK_AUTH_TOKEN "wu5HrRpRE07-dLJErpFe1ZGznUm6iCl7"

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

adc_oneshot_unit_init_cfg_t adc_unit_conf=
{
    .unit_id=ADC_UNIT_1,
    .ulp_mode=ADC_ULP_MODE_DISABLE,
};

adc_oneshot_unit_handle_t adc_unit_handle;

adc_oneshot_chan_cfg_t adc_channel_conf=
{
    .atten=ADC_ATTEN_DB_12,
    .bitwidth=ADC_BITWIDTH_12
};

// Enter your Auth token
char auth[] = "wu5HrRpRE07-dLJErpFe1ZGznUm6iCl7";

//Enter your WIFI SSID and password
char ssid[] = "Oppo X";
char pass[] = "oppo5298";

void get_motorVal_frmCloud(int pin, const char* value)
{
    if(strcmp(value, "1")==0)
    {
        gpio_set_level(relay, 0);
        //lcd funcs, confirm col and row precede
    }
    else
    {
        gpio_set_level(relay, 1);
        //lcd funcs, confirm col and row precede
    }
}


void setup()
{
    gpio_set_direction(LED1, 2);
    gpio_set_direction(LED2, 2);
    gpio_set_direction(LED3, 2);
    gpio_set_direction(LED4, 2);
    gpio_set_direction(trig, 2);
    gpio_set_direction(echo, 1);
    gpio_set_direction(relay, 2);
    gpio_set_direction(buzzer, 2);
    gpio_set_level(relay, 1);

    adc_oneshot_new_unit(&adc_unit_conf, &adc_unit_handle);
    adc_oneshot_config_channel(adc_unit_handle, ADC_CHANNEL_4, &adc_channel_conf);

    blynk_begin(auth, ssid, pass);
    blynk_subscribe(auth, 1, get_motorVal_frmCloud); // subscribe to V1 for motor control
    lcdInit();
    lcd_backlight_on();
    lcd_set_cursor(0, 0);
    lcd_write_string("System");
    lcd_set_cursor(4,1);
    lcd_write_string("Loading..");
    vTaskDelay(pdMS_TO_TICKS(4000));
    lcd_clear();
}

uint32_t pulse_in(uint8_t pin, uint8_t pin_level, uint32_t timeout_us)
{
    uint32_t start_time=esp_timer_get_time();
    uint32_t now=0;

    while(gpio_get_level(echo)==pin_level)
    {
        now = esp_timer_get_time();
        if(now-start_time>timeout_us)
            return 0;
    }

    
    while(gpio_get_level(echo)!=pin_level)
    {
        now = esp_timer_get_time();
        if(now-start_time>timeout_us)
            return 0;
    }
    uint32_t pulse_start=esp_timer_get_time();
    while(gpio_get_level(echo)==pin_level)
    {
        now=esp_timer_get_time();
        if(now-start_time>timeout_us)
            return 0;
    }
    return (now-pulse_start);
}

void ultrasonic()
{
    gpio_set_level(trig, 0);
    esp_rom_delay_us(4);
    gpio_set_level(trig, 1);
    esp_rom_delay_us(10);
    gpio_set_level(trig, 0);
    t=pulse_in(echo, 1, 30000);
    distance = t / 29 / 2;   

}

void write_on_cloud()
{
    blynkDistance = (distance - MaxLevel) * -1;
    if(distance<=MaxLevel)
    {
        blynk_virtual_write(auth, 0, blynkDistance);
    }
    else
    {
        blynk_virtual_write(auth, 0, 0);
    }
}

void update_lcd_buzzer()
{
    lcd_set_cursor(0,0);
    lcd_write_string("WLevel:");
    adc_oneshot_read(adc_unit_handle, ADC_CHANNEL_4, &val);
    if(val>750)
    {
        gpio_set_level(relay, 1);
        blynk_virtual_write(auth, 1, 0);
        lcd_set_cursor(8,0);
        lcd_write_string("Exceeded");
        if(lo==0)
        {
            gpio_set_level(buzzer, 1);
            vTaskDelay(pdMS_TO_TICKS(2000));
            gpio_set_level(buzzer, 0);
            lo++;
        }
        lcd_set_cursor(0,1);
        lcd_write_string("Motor is OFF");
    }
    else
    {
        if(Level1<=distance)
        {
            lcd_set_cursor(8,0);
            lcd_write_string("Very Low");
            gpio_set_level(LED1, 0);
            gpio_set_level(LED2, 0);
            gpio_set_level(LED3, 0);
            gpio_set_level(LED4, 0);
        }
        else if(Level2<=distance && Level1>distance)
        {
            lcd_set_cursor(8,0);
            lcd_write_string("Low");
            lcd_write_string("      ");
            gpio_set_level(LED1, 1);
            gpio_set_level(LED2, 0);
            gpio_set_level(LED3, 0);
            gpio_set_level(LED4, 0);
        }
        else if(Level3<=distance && Level2>distance)
        {
            lcd_set_cursor(8,0);
            lcd_write_string("Medium");
            lcd_write_string("      ");
            gpio_set_level(LED1, 1);
            gpio_set_level(LED2, 1);
            gpio_set_level(LED3, 0);
            gpio_set_level(LED4, 0);
        }
        else if(Level4<=distance && Level3 >distance)
        {
            lcd_set_cursor(8,0);
            lcd_write_string("Almost Full");
            lcd_write_string("      ");
            gpio_set_level(LED1, 1);
            gpio_set_level(LED2, 1);
            gpio_set_level(LED3, 1);
            gpio_set_level(LED4, 0);
        }
        else if(Level4 >distance)
        {
            lcd_set_cursor(8,0);
            lcd_write_string("Full");
            lcd_write_string("      ");
            gpio_set_level(LED1, 1);
            gpio_set_level(LED2, 1);
            gpio_set_level(LED3, 1);
            gpio_set_level(LED4, 1);
        }
    }
}


void app_main(void)
{
    setup();
    while(1)
    {
        blynk_run();
        ultrasonic();
        write_on_cloud();
        update_lcd_buzzer();
    }
    
}
