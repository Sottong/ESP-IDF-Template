#include "main_header.h"

static const char* TAG = "MQTTC_CLIENT";

static const int MQTTC_RECONNECT_MS = 3000;
static const int MQTTC_KEEP_ALIVE_SEC = 5;

enum {  MQTTC_BASE_MSG_MAXLEN = 32,
        MQTTC_TOPIC_MAXLEN = 128,
};

static char lwt_topic[MQTTC_TOPIC_MAXLEN];
static char* lwt_msg = NULL;

static const char* LWT_MSG_UNENCRYPTED = "0";
static const int LWT_QOS = 1;
static const int LWT_RETAIN = 1;


static esp_mqtt_client_handle_t mqttc;
static esp_mqtt_client_config_t mqttc_cfg;

// static char* mqttc_rcv_data;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    char topic[MQTTC_TOPIC_MAXLEN];
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            flag_mqtt_connected = 1;
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            //subscribe topic
            snprintf(topic, MQTTC_TOPIC_MAXLEN, "%s%s%s", TOPIC_HEADER, TOPIC_DEVICE, TOPIC_ONLINE);
            msg_id = esp_mqtt_client_subscribe(client, topic, 1);
            ESP_LOGI(TAG, "Subscribe to %s %s", topic, msg_id == -1 ? "fail" : "success");
            if (msg_id == -1) break;

            //publish topic
            snprintf(topic, MQTTC_TOPIC_MAXLEN, "%s%s%s", TOPIC_HEADER, TOPIC_DEVICE, TOPIC_ONLINE);
            esp_mqtt_client_publish(client, topic, "1", 0, LWT_QOS, LWT_RETAIN);
            ESP_LOGI(TAG, "Publish to %s success", topic);

            break;
        case MQTT_EVENT_DISCONNECTED:
            flag_mqtt_connected = 0;
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
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

        // case MQTT_EVENT_SUBSCRIBED:
        //     ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //     msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        //     ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        //     break;
        // case MQTT_EVENT_UNSUBSCRIBED:
        //     ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        //     break;
        // case MQTT_EVENT_PUBLISHED:
        //     ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        //     break;

    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    if(flag_wifi_sta_connected) mqtt_event_handler_cb(event_data);
}


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
    snprintf(lwt_topic, MQTTC_TOPIC_MAXLEN, "%s%s%s", TOPIC_HEADER, TOPIC_DEVICE, TOPIC_ONLINE);

    mqtt_config.lwt_topic = lwt_topic;
    mqtt_config.lwt_msg = LWT_MSG_UNENCRYPTED;
    mqtt_config.lwt_msg_len = strlen(LWT_MSG_UNENCRYPTED);
    mqtt_config.lwt_qos = LWT_QOS;
    mqtt_config.lwt_retain = LWT_RETAIN;

   

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
