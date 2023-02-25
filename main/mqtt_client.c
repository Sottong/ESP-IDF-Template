#include "main_header.h"

static const char* MQTT_BROKER_URL = "mqtt://industrial.api.ubidots.com";
static const uint32_t MQTT_BROKER_PORT = 1883;


static const char* TAG = "MQTTC";

static const int MQTTC_RECONNECT_MS = 3000;
static const int MQTTC_KEEP_ALIVE_SEC = 5;

// enum {  MQTTC_BASE_MSG_MAXLEN = 32,
//         MQTTC_TOPIC_MAXLEN = 128,
// };

// static const int LWT_QOS = 1;
// static const int LWT_RETAIN = 1;

// static const bool TEST_MQTTC_CRED = false;
static const char* TEST_MQTTC_USER = "BBFF-rqPDXAflwgWtPnBoladP9ZovvTKHyd";
static const char* TEST_MQTTC_PASS = "";

static esp_mqtt_client_handle_t mqttc;
static esp_mqtt_client_config_t mqttc_cfg;

// static char* mqttc_rcv_data;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            flag_mqtt_connected = 1;
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");


            msg_id = esp_mqtt_client_publish(client, "/v1.6/devices/dc-load-logger/tegangan", "120", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            flag_mqtt_connected = 0;
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    if(flag_wifi_sta_connected) mqtt_event_handler_cb(event_data);
}

// static void mqtt_app_start(void)
// {
//     esp_mqtt_client_config_t mqtt_cfg = {
//         .uri = CONFIG_BROKER_URL,
//     };
// #if CONFIG_BROKER_URL_FROM_STDIN
//     char line[128];

//     if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
//         int count = 0;
//         printf("Please enter url of mqtt broker\n");
//         while (count < 128) {
//             int c = fgetc(stdin);
//             if (c == '\n') {
//                 line[count] = '\0';
//                 break;
//             } else if (c > 0 && c < 127) {
//                 line[count] = c;
//                 ++count;
//             }
//             vTaskDelay(10 / portTICK_PERIOD_MS);
//         }
//         mqtt_cfg.uri = line;
//         printf("Broker url: %s\n", line);
//     } else {
//         ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
//         abort();
//     }
// #endif /* CONFIG_BROKER_URL_FROM_STDIN */

//     esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
//     esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
//     esp_mqtt_client_start(client);
// }
static esp_mqtt_client_config_t mqttc_create_config()
{
    esp_mqtt_client_config_t mqtt_config = {0};

    // Essential config
    mqtt_config.uri = MQTT_BROKER_URL;
    mqtt_config.port = MQTT_BROKER_PORT;
    mqtt_config.reconnect_timeout_ms = MQTTC_RECONNECT_MS;
    mqtt_config.username = TEST_MQTTC_USER;
    mqtt_config.password = TEST_MQTTC_PASS;
    mqtt_config.keepalive = MQTTC_KEEP_ALIVE_SEC;

    // LWT config
    // snprintf(lwt_topic, MQTTC_TOPIC_MAXLEN, "%s%s%s", MQTT_TPC_PREFIX, device_uid, TPC_LWT);

    // mqtt_config.lwt_topic = lwt_topic;
    // mqtt_config.lwt_msg = LWT_MSG_UNENCRYPTED;
    // mqtt_config.lwt_msg_len = strlen(LWT_MSG_UNENCRYPTED);
    // mqtt_config.lwt_qos = LWT_QOS;
    // mqtt_config.lwt_retain = LWT_RETAIN;

    // if (MSG_ENCRYPTION_EN) {
    //     free(lwt_msg);
    //     aes128cbc_etm_encrypt(aes_keys, LWT_MSG_UNENCRYPTED, &lwt_msg);
    //     mqtt_config.lwt_msg = lwt_msg;
    //     mqtt_config.lwt_msg_len = strlen(lwt_msg);
    // }

    return mqtt_config;
}

void mqttc_client_init()
{
    
    mqttc_cfg = mqttc_create_config();
     // Start MQTT client
    mqttc = esp_mqtt_client_init(&mqttc_cfg);
    esp_mqtt_client_register_event(mqttc, ESP_EVENT_ANY_ID, mqtt_event_handler, mqttc);
    esp_mqtt_client_start(mqttc);

}
