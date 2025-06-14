#include <stdio.h>
#include "wifi.h"
#include "mqtt.h"
#include "driver/gpio.h"
#include "esp_log.h"

void app_main(void)
{
        wifi_init_sta();
    while (!wifi_is_connected()) {
        printf("Waiting for Wi-Fi...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_LOGI("wifi", "wifi connected");
    //mqtt_app_start();

    while (1) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    }
}