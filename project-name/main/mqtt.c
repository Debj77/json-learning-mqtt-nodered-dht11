// mqtt.c

#include <stdio.h>
#include <string.h>
#include "mqtt_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "mqtt.h"
#include "driver/gpio.h"

static const char *TAG = "MQTT_RGB";
static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            // Subscribe to topics
            esp_mqtt_client_subscribe(client, "esp32/led", 0);      // Same topic as publishing
            esp_mqtt_client_subscribe(client, "esp32/command", 1);  // Different topic
            break;

        case MQTT_EVENT_DATA: {
            ESP_LOGI(TAG, "Topic: %.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "Data: %.*s", event->data_len, event->data);

            // char *json_str = strndup(event->data, event->data_len);
            // cJSON *root = cJSON_Parse(json_str);

            // if (root) {
            //     cJSON *led_dev = cJSON_GetObjectItem(root, "device");
            //     cJSON *led_state = cJSON_GetObjectItem(root, "state");
                    
            //         ESP_LOGI(TAG, "device : %s, state: %d", led_dev->valuestring, led_state->valueint);
            //     }
            // else {
            //     ESP_LOGE(TAG, "Failed to parse JSON");
            // }
            // cJSON_Delete(root);
            break;
        }
        default:
            break;
    }
}

// --- MQTT Publish Task ---
static void mqtt_publish_task(void *pvParameters) {
    while (1) {
        if (client != NULL) {
            // Publish message to "esp32/led"
            const char *message1 = "Hello from ESP32 to esp32/led";
            esp_mqtt_client_publish(client, "esp32/led", message1, 0, 1, 0);

            // Publish message to "esp32/status"
            const char *message2 = "ESP32 status OK";
            esp_mqtt_client_publish(client, "esp32/status", message2, 0, 1, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Publish every 10 seconds
    }
}

void mqtt_app_start(void) {
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker.hivemq.com:1883",
        .broker.address.port = (int)1883
        //.broker.address.transport = MQTT_TRANSPORT_OVER_TCP
        
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    // Start publish task
    xTaskCreate(mqtt_publish_task, "mqtt_publish_task", 4096, NULL, 5, NULL);
}