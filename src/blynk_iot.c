
#include "blynk_iot.h"

static const char *TAG = "BLYNK_IOT";
#define BLYNK_POLL_INTERVAL_MS 2000   // poll every 2s (tune as needed)


// ---- Wi-Fi Connection (like Blynk.begin) ----
//basically what what this func essentially does is to connect to the wifi using the ssid and password provided by the user. 
esp_err_t blynk_begin(const char *auth, const char *ssid, const char *pass) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "Wi-Fi connecting to %s...", ssid);

    // (You can add an event handler to wait until actually connected)

    return ESP_OK;
}

// ---- Virtual Write (like Blynk.virtualWrite) ----
esp_err_t blynk_virtual_write(const char *auth, int pin, int value) {
    char url[256];
    snprintf(url, sizeof(url),
             "http://blynk.cloud/external/api/update?token=%s&pin=V%d&value=%d",
             auth, pin, value);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "Blynk write OK: pin=V%d value=%d status=%d",
                 pin, value, status);
    } else {
        ESP_LOGE(TAG, "Blynk write FAILED: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}

// ---- Virtual Read (like Blynk.syncVirtual / Blynk.write callback) ----
esp_err_t blynk_read(const char *auth, int pin, char *out_value, size_t out_len) {
    char url[256];
    snprintf(url, sizeof(url),
             "http://blynk.cloud/external/api/get?token=%s&pin=V%d",
             auth, pin);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200) {
            int content_length = esp_http_client_get_content_length(client);
            if (content_length > 0 && content_length < out_len) {
                int read_len = esp_http_client_read_response(client, out_value, out_len - 1);
                if (read_len >= 0) {
                    out_value[read_len] = '\0'; // null-terminate
                    ESP_LOGI(TAG, "Blynk read OK: pin=V%d value=%s", pin, out_value);
                }
            }
        } else {
            ESP_LOGE(TAG, "Blynk read FAILED: status=%d", status);
            err = ESP_FAIL;
        }
    } else {
        ESP_LOGE(TAG, "Blynk read FAILED: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}


static int subscription_count = 0;
static const char *g_auth_token = NULL; // store auth for global use

// ---- Register a callback for a pin (like BLYNK_WRITE) ----
//short explanation: This function allows users to subscribe to changes on specific virtual pins. When the value of a subscribed pin changes, the provided callback function is invoked with the pin number and its new value.
esp_err_t blynk_subscribe(const char *auth, int pin, void (*cb)(int, const char *)) {
    if (subscription_count >= 10) return ESP_ERR_NO_MEM;
    g_auth_token = auth; // save auth globally

    subscriptions[subscription_count].pin = pin;
    subscriptions[subscription_count].callback = cb;
    subscriptions[subscription_count].last_value[0] = '\0'; // init empty
    subscription_count++;
    ESP_LOGI(TAG, "Subscribed to V%d", pin);
    return ESP_OK;
}

// ---- FreeRTOS Task that polls Blynk cloud ----
static void blynk_task(void *pv) {
    char value[32];
    while (1) {
        for (int i = 0; i < subscription_count; i++) {
            if (blynk_read(g_auth_token, subscriptions[i].pin, value, sizeof(value)) == ESP_OK) 
            {
                if (strcmp(value, subscriptions[i].last_value) != 0) 
                {
                    // Value changed → trigger callback
                    strcpy(subscriptions[i].last_value, value);
                    if (subscriptions[i].callback) 
                    {
                        subscriptions[i].callback(subscriptions[i].pin, value);
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(BLYNK_POLL_INTERVAL_MS));
    }
}

// ---- Start Blynk task (like Blynk.run) ----
void blynk_run(void) {
    xTaskCreatePinnedToCore(blynk_task, "blynk_task", 4096, NULL, 5, NULL, 1);
}

