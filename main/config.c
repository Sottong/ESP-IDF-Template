#include "main_header.h"


/* Firmware */


/*  Wifi    */


/*  MQTT    */

// Parameter
const char* MQTT_BROKER_URL = "mqtt://industrial.api.ubidots.com";
const uint32_t MQTT_BROKER_PORT = 1883;

// Kredensial
const char* TEST_MQTTC_USER = "BBFF-rqPDXAflwgWtPnBoladP9ZovvTKHyd";
const char* TEST_MQTTC_PASS = "";

// Topic 
const char* TOPIC_HEADER = "/v1.6/devices/";

const char* TOPIC_DEVICE = "dc-load-logger/";

const char* TOPIC_SEN_VOL = "tegangan";
const char* TOPIC_SEN_CRN = "arus";
const char* TOPIC_SEN_PWR = "power";

const char* TOPIC_ONLINE = "online";


