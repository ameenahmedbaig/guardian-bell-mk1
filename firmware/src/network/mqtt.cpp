// --- ESP-IDF certificate bundle for TLS validation ---
#include "esp_crt_bundle.h"


// === project headers ===
// --- corresponding header ---
#include "mqtt.h"

// --- secrets_example.h for reference ---
#include "secrets.h"

// --- network ---
#include "wifi.h"

// --- utilities ---
#include "debug.h"
#include "error.h"


/// === WIFI client setup (TLS-encrypted) ===
WiFiClientSecure mqttWifiClient;


/// === MQTT Setup ===
PubSubClient mqtt(mqttWifiClient);


/// === ensure connection to MQTT  ===
void ensureMQTT() {
    /// --- enable TLS certificate validation ---
    mqttWifiClient.setCACertBundle(esp_crt_bundle_attach);

    /// --- attempt to connect for 5 seconds ---
    unsigned long attemptConnection_startTime = millis();
    while (!mqtt.connected() && millis() - attemptConnection_startTime < 5000) {
        DBG_PRINTLN("Not connected to MQTT");
        delay(50);
        mqtt.connect(
            "smart-doorbell",
            MQTT_USER,
            MQTT_PASS
        );
    }

    if (!mqtt.connected()) {
        error("Failed to connect to MQTT in time", false);
    }
    else {
        DBG_PRINTLN("Connected to MQTT");
    }
}