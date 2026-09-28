
# ESP32 HomeKit + ESP-NOW Light Controller

A wireless smart-lighting system using an **ESP32 as the central HomeKit controller** and **ESP8266 nodes as wireless light controllers**.

The ESP32 connects to the local Wi-Fi network and integrates with **Apple HomeKit using HomeSpan**. It communicates with ESP8266 devices using **ESP-NOW**, allowing multiple lights to be controlled without requiring each ESP8266 to connect directly to the router.

The system can be controlled from the **Apple Home app** or using **Siri**.

---

## Architecture

```text
                 Apple Home / Siri
                         │
                         │ Wi-Fi / HomeKit
                         ▼
                ┌─────────────────┐
                │      ESP32      │
                │                 │
                │    HomeSpan     │
                │    HomeKit      │
                │                 │
                │    ESP-NOW      │
                └────────┬────────┘
                         │
                         │ ESP-NOW
                         ▼
                ┌─────────────────┐
                │   ESP8266 #1    │
                │                 │
                │   GPIO / Relay  │
                └────────┬────────┘
                         │
                         ▼
                   Outdoor Light
```

Additional ESP8266 nodes can be added later:

```text
                         ESP32
                           │
              ┌────────────┼────────────┐
              │            │            │
           ESP8266 #1   ESP8266 #2   ESP8266 #3
              │            │            │
           Light 1      Light 2      Light 3
```

---

## Features

* Apple HomeKit integration
* Siri voice control
* ESP32 central controller
* ESP-NOW communication
* ESP8266 wireless light nodes
* Relay-based light control
* Multiple ESP8266 nodes can be added
* No MQTT server required
* No cloud server required for the local control path
* 2.4 GHz Wi-Fi
* HomeKit pairing using HomeSpan

---

## Hardware

### Controller

* ESP32 development board

### Light Node

* NodeMCU ESP8266
* 1-channel relay module
* Outdoor light

### Network

* 2.4 GHz Wi-Fi router

---

## Pin Configuration

For ESP8266 #1:

| ESP8266    | Function  |
| ---------- | --------- |
| D1 / GPIO5 | Relay IN  |
| GND        | Relay GND |
| VIN / 5V   | Relay VCC |

The onboard LED is no longer used for the light output.

---

## Relay Connections

A typical relay module has three load terminals:

```text
COM = Common
NO  = Normally Open
NC  = Normally Closed
```

For a light that should normally remain OFF and turn ON when commanded, the typical configuration is:

```text
COM + NO
```

Conceptually:

```text
             Relay
          ┌──────────┐
Live ─────┤ COM      │
          │     NO ──┼────── Light
          └──────────┘
```

When the relay is OFF:

```text
COM ──X── NO

Light OFF
```

When the relay is ON:

```text
COM ────── NO

Light ON
```

### ⚠️ Mains Safety

If the outdoor light operates from mains voltage, such as 230 V AC, do not work on the mains wiring while energized.

Use an appropriately rated relay, proper insulation and enclosure, and appropriate electrical protection. The mains-side installation should be performed by a qualified electrician.

---

# Software

## ESP32

The ESP32 uses:

* Arduino framework
* HomeSpan
* Wi-Fi
* ESP-NOW

The ESP32 acts as:

1. Wi-Fi client
2. HomeKit accessory
3. ESP-NOW transmitter

---

## ESP8266

The ESP8266 acts as:

1. ESP-NOW receiver
2. Relay controller
3. Light controller

---

# Communication

The ESP32 sends a small command structure to each ESP8266.

```cpp
struct Command {
  uint8_t device;
  char action[8];
};
```

Example:

```text
device = 1
action = "ON"
```

or:

```text
device = 1
action = "OFF"
```

The ESP8266 checks the device ID before processing the command.

---

# Current ESP8266 #1 Configuration

ESP8266 #1 uses:

```text
Device ID: 1
Relay Pin: GPIO5 / D1
ESP-NOW Channel: 6
```

The ESP32 and ESP8266 must operate on the same Wi-Fi channel for ESP-NOW communication.

Current network arrangement:

```text
Wi-Fi Channel: 6
```

---

# ESP32 Configuration

Before uploading the ESP32 code, configure your own Wi-Fi credentials:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
```

Do **not** commit real Wi-Fi credentials to GitHub.

A better approach is to keep credentials in a separate local configuration file that is listed in `.gitignore`.

---

# HomeKit Setup

The ESP32 uses HomeSpan to create a HomeKit light accessory.

The accessory can be paired with Apple Home using the HomeSpan pairing code configured in the ESP32 firmware.

After pairing, the light appears in the Apple Home app.

Example:

```text
Apple Home
    │
    └── ESP32 Light
```

The light can then be controlled using:

* Apple Home app
* Siri
* HomeKit automations

Example Siri commands:

```text
"Hey Siri, turn on the outdoor light."

"Hey Siri, turn off the outdoor light."
```

---

# ESP-NOW Flow

When the HomeKit light is turned ON:

```text
Apple Home
     ↓
HomeSpan
     ↓
ESP32
     ↓
ESP-NOW
     ↓
ESP8266 #1
     ↓
GPIO5 / D1
     ↓
Relay
     ↓
Outdoor Light ON
```

When the light is turned OFF:

```text
Apple Home
     ↓
HomeSpan
     ↓
ESP32
     ↓
ESP-NOW
     ↓
ESP8266 #1
     ↓
GPIO5 / D1
     ↓
Relay
     ↓
Outdoor Light OFF
```

---

# ESP8266 Serial Output

When a command is received, the ESP8266 prints:

```text
Received command: ON
Outdoor Light ON
```

For OFF:

```text
Received command: OFF
Outdoor Light OFF
```

---

# Adding More ESP8266 Nodes

The project is designed to support multiple ESP8266 devices.

For example:

```text
Device 1 → Outdoor Light
Device 2 → Bedroom Light
Device 3 → Kitchen Light
```

Each device can have its own MAC address and device ID.

Example:

```cpp
#define DEVICE_ID 1
```

The ESP32 can then send commands to the appropriate ESP8266.

Future HomeKit accessories could appear as:

```text
Outdoor Light
Bedroom Light
Kitchen Light
```

---

# Project Status

### Completed

* [x] ESP32 Wi-Fi connection
* [x] ESP32 ESP-NOW transmission
* [x] ESP8266 ESP-NOW reception
* [x] HomeSpan HomeKit integration
* [x] Apple Home pairing
* [x] Siri control
* [x] ESP8266 relay control
* [x] Outdoor light control

### Planned

* [ ] Add ESP8266 #2
* [ ] Add ESP8266 #3
* [ ] Multiple HomeKit light accessories
* [ ] Improved Wi-Fi reconnection
* [ ] Improved HomeKit availability after ESP32 restart
* [ ] ESP-NOW error handling
* [ ] Relay status feedback
* [ ] Optional brightness control for compatible lights

---

# Project Structure

A possible repository structure:

```text
ESP32-HomeKit-ESP-NOW-Light-Controller/
│
├── ESP32_HomeKit_Controller/
│   └── ESP32_HomeKit_Controller.ino
│
├── ESP8266_Light_Node/
│   └── ESP8266_Light_Node.ino
│
├── .gitignore
│
└── README.md
```

---

# Requirements

### Arduino IDE

Install:

* ESP32 board package
* ESP8266 board package
* HomeSpan library

The ESP32 firmware was developed with the ESP32 Arduino core 3.x API.

---

# Notes

ESP-NOW and Wi-Fi channel configuration are important.

The ESP32 is connected to the Wi-Fi router, so its ESP-NOW communication must use the same channel as the ESP8266 nodes.

In the current setup:

```text
Router       → Channel 6
ESP32        → Channel 6
ESP8266 #1   → Channel 6
```

If the channels do not match, ESP-NOW transmission can fail with an error similar to:

```text
Peer channel is not equal to the home channel
```

---

# Disclaimer

This project is intended for experimentation and learning with ESP32, ESP8266, HomeKit, and ESP-NOW.

When controlling mains-powered lighting, follow appropriate electrical safety practices and local electrical regulations. Use correctly rated components and have mains wiring performed by a qualified electrician.

---

## License

Choose a license appropriate for your project. MIT is a common choice for open-source hardware/software projects.
