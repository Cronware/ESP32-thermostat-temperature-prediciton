#include <WiFi.h>
#include <PubSubClient.h>
#include <FS.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

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
WebServer server(80);
WebSocketsServer webSocket(81); // WebSocket on port 81

// Store received values
String currentTemp = "--";
String humidity = "--";
String estimatedTime = "--";
float setpointTemp = 24.0;  // Default setpoint

void sendWebSocketUpdate() {
    String json = "{";
    json += "\"temperature\":\"" + currentTemp + "\",";
    json += "\"humidity\":\"" + humidity + "\",";
    json += "\"estimated_time\":\"" + estimatedTime + "\"}";
    webSocket.broadcastTXT(json);  // Send to all connected clients
}

void callback(char* topic, byte* message, unsigned int length) {
    char msg[length + 1];
    memcpy(msg, message, length);
    msg[length] = '\0';

    if (strcmp(topic, mqtt_topic_setpoint) == 0) {
        setpointTemp = atof(msg);
        Serial.print("New Setpoint Received: ");
        Serial.println(setpointTemp);
    }
    else if (strcmp(topic, mqtt_topic_temp) == 0) {
        currentTemp = msg;
        Serial.print("Current Temperature Received: ");
        Serial.println(currentTemp);
        sendWebSocketUpdate();
    }
    else if (strcmp(topic, mqtt_topic_humidity) == 0) {
        humidity = msg;
        Serial.print("Humidity Received: ");
        Serial.println(humidity);
        sendWebSocketUpdate();
    }
    else if (strcmp(topic, mqtt_topic_estimated) == 0) {
        estimatedTime = msg;
        Serial.print("Estimated Time Received: ");
        Serial.println(estimatedTime);
        sendWebSocketUpdate();
    }
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        Serial.print("WebSocket Received: ");
        Serial.println((char*)payload);

        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            if (doc.containsKey("setpoint")) {
                float receivedSetpoint = doc["setpoint"];
                Serial.print("New Setpoint via WebSocket: ");
                Serial.println(receivedSetpoint);

                // Publish to MQTT
                client.publish(mqtt_topic_setpoint, String(receivedSetpoint).c_str());
            }
        } else {
            Serial.println("Error parsing WebSocket JSON!");
        }
    }
}

// Reconnect to MQTT
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32_Setpoint_Client")) {
            Serial.println("Connected to MQTT");
            client.subscribe(mqtt_topic_setpoint);
            client.subscribe(mqtt_topic_temp);
            client.subscribe(mqtt_topic_humidity);
            client.subscribe(mqtt_topic_estimated);
        } else {
            Serial.print("Failed, rc=");
            Serial.println(client.state());
            delay(5000);
        }
    }
}

// Serve the HTML page from LittleFS
void handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

void setup() {
    Serial.begin(115200);

    // Start LittleFS
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed");
        return;
    }

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

    // MQTT Setup
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    // Start Web Server
    server.on("/", handleRoot);
    server.begin();
    Serial.println("Web Server Started!");

    // Start WebSockets
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    Serial.println("WebSocket Server Started!");
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    server.handleClient();
    webSocket.loop();
}
