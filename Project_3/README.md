# ESP8266 NTP Smart Alarm Clock

A feature-rich internet-connected alarm clock built with ESP8266 that synchronizes time via NTP (Network Time Protocol) - no RTC module required!

![Project Banner](link-to-your-image)

## 📸 Features

- **🕐 Accurate Time Sync** - Uses NTP to get precise time from the internet
- **🌐 Web Interface** - Control everything from your phone or computer
- **⏰ Smart Alarm** - Set alarm time, enable/disable from web
- **📟 Dual Display** - 16x2 LCD for messages + TM1637 7-segment for time
- **🔊 Audio Alarm** - Piezo buzzer with 4kHz beep pattern
- **💡 Visual Alerts** - LCD backlight & segment display blink during alarm
- **🛑 Stop Button** - Physical button to silence the alarm
- **📝 Scrolling Text** - Send custom messages to the LCD
- **💡 Backlight Control** - Turn LCD backlight ON/OFF from web

## 🛠️ Components Required

| Component | Quantity |
|-----------|----------|
| ESP8266 (NodeMCU) | 1 |
| 16x2 I2C LCD Display | 1 |
| TM1637 7-Segment Display | 1 |
| Piezo Buzzer | 1 |
| Push Button | 1 |
| Jumper Wires | As needed |
| Breadboard | 1 |

## 🔌 Pin Connections

| Component | Pin | ESP8266 Pin |
|-----------|-----|-------------|
| TM1637 CLK | D5 | GPIO14 |
| TM1637 DIO | D6 | GPIO12 |
| LCD I2C SDA | D2 | GPIO4 |
| LCD I2C SCL | D1 | GPIO5 |
| Buzzer | D7 | GPIO13 |
| Stop Button | D8 | GPIO15 |

## 📦 Libraries Required

Install these libraries from Arduino Library Manager:

- `LiquidCrystal_I2C` by Frank de Brabander
- `TM1637Display` by Avishay Orpaz
- `NTPClient` by Fabrice Weinberg
- `ESP8266WiFi` (built-in)
- `ESP8266WebServer` (built-in)
- `WiFiUdp` (built-in)

## ⚙️ Setup Instructions

### 1. Clone the Repository
```bash
2. Configure WiFi
Open the code and update these lines:

cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
3. Upload Code
Select board: NodeMCU 1.0 (ESP-12E Module)

Select port: COMx (your port)

Click Upload

4. Find IP Address
Open Serial Monitor (115200 baud) and note the IP address displayed.

5. Access Web Interface
Open a browser and enter the IP address.

🌐 Web Interface Features
Send custom scrolling messages to LCD

Set alarm time (Hour: 0-23, Minute: 0-59)

Enable/Disable alarm

Turn LCD backlight ON/OFF

View alarm status

📱 Usage Guide
Setting Alarm
Open web interface (IP address)

Enter Hour (0-23) and Minute (0-59)

Click "Set Alarm"

Click "Enable Alarm"

Alarm will trigger at the set time

Stopping Alarm
Press the physical STOP_BUTTON on D8

OR click "Disable Alarm" on web interface

Sending Messages
Type message in the text box

Click "Send Fact"

Message scrolls on LCD

Backlight Control
Click "Backlight ON" or "Backlight OFF" on web interface
git clone https://github.com/Neeraj-7-7-7/ESP8266-Alarm-Clock.git
