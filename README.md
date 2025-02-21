# ESP32 Thermostat - Real-Time MQTT-Based Web Thermostat
🚀 An IoT thermostat using ESP32, WebSockets, and MQTT for real-time temperature control and monitoring.

## Features
- ✅ Web-based Thermostat (Adjust temperature from a browser)
- ✅ Real-time Updates using WebSockets
- ✅ ESP32 + MQTT Communication
- ✅ Displays Current Temperature & Humidity
- ✅ Predicts Estimated Time for Setpoint & Shows in HH:MM Format

## Project Structure
```graphql
ESP32-Thermostat/
│── data/                 # HTML Files for LittleFS
│   ├── index.html
│── src/                  # ESP32 Firmware Code
│   ├── esp32_thermostat.ino
│── README.md             # Project Documentation
│── .gitignore            # Ignore compiled files
```

## Requirements
1️⃣ Hardware Needed:
- ESP32 Dev Board
- SHT21 Temperature & Humidity Sensor
- WiFi Network
- MQTT Broker (Local or Cloud)
2️⃣ Software Needed:
Arduino IDE
- ESP32 Board Support (ESP32 by Espressif Systems via Boards Manager)
- Arduino Libraries:
  - PubSubClient (Library Manager → Search "PubSubClient" → Install)
  - WebSocketsServer (Library Manager → Search "WebSocketsServer" → Install)
  - ArduinoJson (Library Manager → Search "ArduinoJson" → Install)
  - LittleFS for ESP32 (Library Manager → Search "LittleFS_ESP32")

## How to Setup
### 1️⃣ Flash the ESP32 Code
1. Open Arduino IDE.
2. Install required libraries (see above).
3. Open esp32_thermostat.ino.
4. Modify WiFi & MQTT details:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
```
5. Connect ESP32 and Upload the Code.
