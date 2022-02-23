#include <PubSubClient.h> // for MQTT

char mqtt_server[15] = ""; // IPv4 only supported
char mqtt_port[5]    = 1883;
char mqtt_user       = "";
char mqtt_pass       = "";

// MQTT Client
PubSubClient mqttClient(wifiClient);

void TryConnectMQTT ()
{
    delay(10);

    if (mqtt_server != String("") && WiFi.status() == WL_CONNECTED && !mqttClient.connected())
    {
        mqttClient.setServer(mqtt_server , mqtt_port);
        mqttClient.setCallback(handleMQTT);

        // Connect to mqtt server
        if (mqtt_user != String(""))
        {
            mqttClient.connect(
        }
    }
}
