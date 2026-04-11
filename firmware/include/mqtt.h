#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern WiFiClientSecure mqttWifiClient;

extern PubSubClient mqtt;

void ensureMQTT();