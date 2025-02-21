#include <Wire.h>
#include <SHT2x.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const char* mqtt_topic_setpoint = "thermostat/setpoint";
const char* mqtt_topic_temp = "thermostat/temperature";
const char* mqtt_topic_humidity = "thermostat/humidity";
const char* mqtt_topic_estimated = "thermostat/estimated_time";

WiFiClient espClient;
PubSubClient client(espClient);
SHT2x sht;

// Temperature Tracking for Prediction
#define MAX_HISTORY 20
float temperatureHistory[MAX_HISTORY] = {0}; // Store last 20 values
int historyIndex = 0;
float setpointTemp = 24.0;  // Default setpoint

void callback(char* topic, byte* message, unsigned int length) {
    char msg[length + 1];
    memcpy(msg, message, length);
    msg[length] = '\0';

    Serial.print("MQTT Message Received on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(msg);

    if (strcmp(topic, mqtt_topic_setpoint) == 0) {
        setpointTemp = atof(msg);
        Serial.print("New Setpoint Received: ");
        Serial.println(setpointTemp);
    }
}


void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32_SHT21_Client")) {
            Serial.println("Connected to MQTT!");
            client.subscribe(mqtt_topic_setpoint);
            Serial.println("Subscribed to thermostat/setpoint");
        } else {
            Serial.print("Failed, rc=");
            Serial.println(client.state());
            Serial.println("Retrying in 5 seconds...");
            delay(5000);
        }
    }
}


void setup() {
    Serial.begin(115200);
    Wire.begin();
    sht.begin();
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected to Wi-Fi");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup MQTT
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    static unsigned long lastMillis = 0;
    if (millis() - lastMillis > 5000) {  // Update every 5 seconds
        lastMillis = millis();
        sht.read();
        // Read Temperature & Humidity
        float temperature = sht.getTemperature();
        float humidity = sht.getHumidity();

        Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");

        // Store last 20 temperature readings
        temperatureHistory[historyIndex] = temperature;
        historyIndex = (historyIndex + 1) % MAX_HISTORY;

        // Calculate Estimated Time to Reach Setpoint
        float estimatedTime = predictTimeToSetpoint();

        // Publish Data to MQTT
        client.publish(mqtt_topic_temp, String(temperature).c_str());
        client.publish(mqtt_topic_humidity, String(humidity).c_str());
        client.publish(mqtt_topic_estimated, String(estimatedTime).c_str());

        Serial.print("Estimated Time to Setpoint: "); Serial.print(estimatedTime); Serial.println(" sec");
    }
}

// Function to Predict Time to Reach Setpoint
float predictTimeToSetpoint() {
    float currentTemp = temperatureHistory[(historyIndex - 1 + MAX_HISTORY) % MAX_HISTORY];

    if (currentTemp == setpointTemp) return 0;  // Already at setpoint

    float avgRate = 0;
    int validReadings = 0;

    // Calculate the average rate of temperature change
    for (int i = 1; i < MAX_HISTORY; i++) {
        int index = (historyIndex - i + MAX_HISTORY) % MAX_HISTORY;
        int prevIndex = (index - 1 + MAX_HISTORY) % MAX_HISTORY;

        if (temperatureHistory[index] != 0 && temperatureHistory[prevIndex] != 0) {
            float rate = temperatureHistory[index] - temperatureHistory[prevIndex];
            avgRate += rate;
            validReadings++;
        }
    }

    if (validReadings == 0 || avgRate == 0) return -1; // Not enough data

    avgRate /= validReadings; // Average change per sample (5s interval)
    float timeToSetpoint = (setpointTemp - currentTemp) / avgRate * 5; // Convert to seconds

    return timeToSetpoint > 0 ? timeToSetpoint : -1;
}
