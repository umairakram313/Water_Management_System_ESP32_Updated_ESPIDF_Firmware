#ifndef BLYNK_IOT_H
#define BLYNK_IOT_H

#include "esp_err.h"
#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_client.h"


typedef struct {
    int pin;
    char last_value[32];  // cache last read value
    void (*callback)(int pin, const char *value); // user callback
} blynk_subscription_t;

static blynk_subscription_t subscriptions[10]; // up to 10 pins
static int subscription_count;
static const char *g_auth_token; // store auth for global use

// Initialize Wi-Fi (replacement for Blynk.begin)
esp_err_t blynk_begin(const char *auth, const char *ssid, const char *pass);

// Virtual Write (replacement for Blynk.virtualWrite)
esp_err_t blynk_virtual_write(const char *auth, int pin, int value);

// TODO: later -> add blynk_read(), blynk_run(), etc.
esp_err_t blynk_read(const char *auth, int pin, char *out_value, size_t out_len);

esp_err_t blynk_subscribe(const char *auth, int pin, void (*cb)(int, const char *));

// ---- FreeRTOS Task that polls Blynk cloud ----
static void blynk_task(void *pv);

// ---- Start Blynk task (like Blynk.run) ----
void blynk_run(void);

#endif // BLYNK_IOT_H
