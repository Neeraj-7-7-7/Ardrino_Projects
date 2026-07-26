# 🔐 ESP8266 Smart Door Lock

A Wi-Fi-enabled smart door locking system built using an **ESP8266**, **dual relay module**, and a **DC geared motor**. The system allows the door to be locked and unlocked remotely through a web interface or integrated with a custom **JARVIS voice assistant**.

## ✨ Features

* 🔒 Remote door locking through Wi-Fi
* 🔓 Remote door unlocking through Wi-Fi
* 🌐 Built-in ESP8266 web server
* 🖥️ Simple web interface with Lock and Unlock buttons
* 🛡️ Motor protection logic to prevent repeated locking or unlocking
* ⚡ Dual-relay control for bidirectional motor operation
* ⏱️ Configurable motor runtime
* 🔋 Wi-Fi power optimization using light sleep
* 🤖 Can be integrated with a Python-based JARVIS voice assistant
* 📡 Works over a local Wi-Fi network

## 🛠️ Hardware Used

* ESP8266 NodeMCU
* 2-Channel Relay Module
* DC Geared Motor
* Li-ion Battery
* H-Bridge / Relay-based motor control circuit
* Door Lock Mechanism

## 💻 Software & Technologies

* Arduino IDE
* C/C++
* ESP8266WiFi Library
* ESP8266WebServer Library
* HTTP Requests
* Wi-Fi

## 📌 Pin Configuration

| Component    | ESP8266 Pin |
| ------------ | ----------- |
| Lock Relay   | D1 (GPIO5)  |
| Unlock Relay | D2 (GPIO4)  |

## 🌐 Web Controls

After connecting to Wi-Fi, the ESP8266 displays its assigned IP address in the Serial Monitor.

Open the IP address in a browser:

```text
http://ESP_IP/
```

Use the available buttons to control the door:

```text
/lock
```

```text
/unlock
```

Example:

```text
http://192.168.x.x/lock
```

## 🤖 JARVIS Voice Assistant Integration

This project can be integrated with a Python-based JARVIS voice assistant. JARVIS can send HTTP requests to the ESP8266 to control the door.

Example:

```python
import requests

ESP_IP = "192.168.x.x"

def lock_door():
    try:
        requests.get(f"http://{ESP_IP}/lock", timeout=10)
        print("Lock command sent")
    except requests.exceptions.RequestException:
        print("Unable to reach door lock system")

def unlock_door():
    try:
        requests.get(f"http://{ESP_IP}/unlock", timeout=10)
        print("Unlock command sent")
    except requests.exceptions.RequestException:
        print("Unable to reach door lock system")
```

This allows voice commands such as:

> "JARVIS, lock the door."

> "JARVIS, unlock the door."

## 🛡️ Safety & Motor Protection

The system maintains the current door state in software. If the door is already locked, another lock command is ignored. Similarly, an unlock command is ignored when the door is already unlocked.

This prevents unnecessary motor operation and reduces the risk of damaging the geared locking mechanism.

The motor automatically stops after the configured runtime.

## 🔋 Power Optimization

The ESP8266 is configured to reduce power consumption while keeping the Wi-Fi connection available for remote control.

Power-saving features include:

```cpp
WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
WiFi.setOutputPower(10.0);
system_update_cpu_freq(80);
```

These settings reduce power consumption while maintaining the ability to receive commands over Wi-Fi.

## 🚀 Future Improvements

* [ ] Store door state permanently using EEPROM
* [ ] Add door status API for JARVIS
* [ ] Add physical lock/unlock buttons
* [ ] Add limit switches for physical position detection
* [ ] Add battery voltage monitoring
* [ ] Add automatic low-battery alerts
* [ ] Improve motor protection with limit switches
* [ ] Add authentication for web commands
* [ ] Upgrade to a more secure communication protocol
* [ ] Add ESP32/BLE-based low-power control

## ⚠️ Disclaimer

This is a DIY electronics project intended for educational and experimental purposes. Always use appropriate motor drivers, relays, fuses, battery protection circuits, and mechanical safety mechanisms when building a physical door lock system.

## 👨‍💻 Author

**Neeraj Ranjan**

B.Tech Computer Science & Engineering Student
Noida International University

---

⭐ If you find this project interesting, consider giving the repository a star!
