# ESP32 Thermostat - Real-Time MQTT-Based Web Thermostat
🚀 An IoT thermostat using ESP32, WebSockets, and MQTT for real-time temperature control and monitoring.

![image](https://github.com/user-attachments/assets/3c4af350-2546-450d-89d7-016c6a41818f)

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

### 2️⃣ Upload index.html to ESP32 (LittleFS)
1. Install the ESP32 LittleFS Plugin:
   - Download the plugin: ESP32 LittleFS Plugin
   - Copy it to Arduino/tools/ESP32FS/tool/
2. In Arduino IDE:
   - Go to Tools → ESP32 Sketch Data Upload.
   - It will upload index.html to ESP32.

### 3️⃣ Set Up MQTT Broker
- If using Mosquitto MQTT on Raspberry Pi/PC, install it:
```bash
sudo apt install mosquitto mosquitto-clients
```
- Start the broker:
```bash
sudo systemctl start mosquitto
```
- Subscribe to MQTT topics to test:
```bash
mosquitto_sub -h YOUR_MQTT_BROKER_IP -t "thermostat/#" -v
```

### 4️⃣ Access the Thermostat Dashboard
1. Open Serial Monitor (115200 baud).
2. Get the ESP32 IP Address.
3. Open a browser and go to:
```cpp
http://ESP32_IP/
```
4. Adjust the Set Temperature and see real-time updates.
