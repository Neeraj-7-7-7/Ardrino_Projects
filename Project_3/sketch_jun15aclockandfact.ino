#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TM1637Display.h>

#define CLK D5
#define DIO D6

// Alarm Global Variables
#define BUZZER D7       // Using D7 as you confirmed it works
#define STOP_BUTTON D8

bool alarmEnabled = true;
bool alarmRinging = false;

int alarmHour = 7;
int alarmMinute = 0;
int lastAlarmDay = -1;

// Debounce variables
unsigned long lastButtonPress = 0;

// NTP update control
unsigned long lastNTPUpdate = 0;

// Colon blinking
bool colonState = true;
unsigned long lastColonBlink = 0;

// Store previous LCD lines to minimize flicker
String lastLine1 = "";
String lastLine2 = "";

// For segment display blinking during alarm
bool segmentDisplayOn = true;
unsigned long lastSegmentBlink = 0;

// For LCD backlight blinking during alarm
unsigned long lastLcdBlink = 0;
bool backlightState = true;

// For alarm pattern control
unsigned long lastAlarmPatternTime = 0;
int alarmPatternStep = 0;

TM1637Display segDisplay(CLK, DIO);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "NEERAJ";
const char* password = "12345678";

ESP8266WebServer server(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800);

String factText = "Welcome to ESP Clock!";
int scrollPos = 0;

// Helper function to update LCD without flicker
void updateLCD(String line1, String line2) {
  if (line1 != lastLine1) {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lastLine1 = line1;
  }
  
  if (line2 != lastLine2) {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lastLine2 = line2;
  }
}

// YOUR WORKING BEEP FUNCTION
void beep(int durationMs) {
  unsigned long endTime = millis() + durationMs;
  while (millis() < endTime) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(125); // 4 kHz
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(125);
  }
}

// Start alarm
void startAlarm() {
  Serial.println("ALARM TRIGGERED!");
  alarmRinging = true;
  alarmPatternStep = 0;
  lastAlarmPatternTime = millis();
  lastSegmentBlink = millis();
  lastLcdBlink = millis();
  segmentDisplayOn = true;
  backlightState = true;
}

// Stop alarm
void stopAlarm() {
  Serial.println("ALARM STOPPED!");
  alarmRinging = false;
  digitalWrite(BUZZER, LOW);
  segDisplay.setBrightness(7);  // Restore normal brightness
  lcd.backlight();  // Ensure LCD backlight is on
  segmentDisplayOn = true;
}

void handleRoot() {
  String page = "<html><body>"
  "<h2>ESP8266 Fact Display & Alarm Clock</h2>"
  "<form action='/set'>"
  "<input type='text' name='msg' style='width:300px'>"
  "<input type='submit' value='Send Fact'>"
  "</form><br>"
  "<a href='/lighton'><button>Backlight ON</button></a>"
  "<a href='/lightoff'><button>Backlight OFF</button></a><br><br>"
  "<h3>Alarm Settings</h3>"
  "<form action='/setalarm'>"
  "Hour (0-23): <input type='number' name='hour' min='0' max='23' value='" + String(alarmHour) + "'><br>"
  "Minute (0-59): <input type='number' name='minute' min='0' max='59' value='" + String(alarmMinute) + "'><br>"
  "<input type='submit' value='Set Alarm'>"
  "</form><br>"
  "<a href='/alarmon'><button>Enable Alarm</button></a>"
  "<a href='/alarmoff'><button>Disable Alarm</button></a><br><br>"
  "Alarm Status: <b>" + String(alarmEnabled ? "ENABLED" : "DISABLED") + "</b><br>"
  "Alarm Time: <b>" + String(alarmHour) + ":" + (alarmMinute < 10 ? "0" : "") + String(alarmMinute) + "</b><br>"
  "Alarm Ringing: <b>" + String(alarmRinging ? "YES" : "NO") + "</b>"
  "</body></html>";
  
  server.send(200, "text/html", page);
}

void handleSet() {
  if(server.hasArg("msg")) {
    factText = server.arg("msg");
    scrollPos = 0;
  }
  server.sendHeader("Location","/");
  server.send(302,"text/plain","");
}

void handleBacklightOn() {
  lcd.backlight();
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleBacklightOff() {
  lcd.noBacklight();
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleSetAlarm() {
  if(server.hasArg("hour") && server.hasArg("minute")) {
    alarmHour = server.arg("hour").toInt();
    alarmMinute = server.arg("minute").toInt();
    
    if(alarmHour < 0) alarmHour = 0;
    if(alarmHour > 23) alarmHour = 23;
    if(alarmMinute < 0) alarmMinute = 0;
    if(alarmMinute > 59) alarmMinute = 59;
    
    Serial.print("Alarm set to: ");
    Serial.print(alarmHour);
    Serial.print(":");
    Serial.println(alarmMinute);
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleAlarmOn() {
  alarmEnabled = true;
  Serial.println("Alarm ENABLED");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleAlarmOff() {
  alarmEnabled = false;
  stopAlarm();
  Serial.println("Alarm DISABLED");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nESP8266 Starting...");

  pinMode(BUZZER, OUTPUT);
  pinMode(STOP_BUTTON, INPUT_PULLUP);
  digitalWrite(BUZZER, LOW);

  lcd.init();
  lcd.backlight();

  segDisplay.setBrightness(7);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("Connected");
  lastLine1 = "Connected";
  lastLine2 = "";

  Serial.println("\nWiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  timeClient.update();
  
  Serial.print("Current time: ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.println(timeClient.getMinutes());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/lighton", handleBacklightOn);
  server.on("/lightoff", handleBacklightOff);
  server.on("/setalarm", handleSetAlarm);
  server.on("/alarmon", handleAlarmOn);
  server.on("/alarmoff", handleAlarmOff);

  server.begin();
  Serial.println("HTTP server started");

  lcd.clear();
  lastLine1 = "";
  lastLine2 = "";
  
  // Test beep on startup
  Serial.println("Testing buzzer...");
  beep(100);
  delay(200);
  beep(100);
}

void loop() {
  server.handleClient();
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.reconnect();
    delay(1000);
  }
  
  if (millis() - lastNTPUpdate > 60000) {
    timeClient.update();
    lastNTPUpdate = millis();
  }
  
  // Check for stop button
  if (alarmRinging && digitalRead(STOP_BUTTON) == LOW && millis() - lastButtonPress > 200) {
    stopAlarm();
    lastButtonPress = millis();
  }
  
  // Update colon blinking every 500ms
  if (millis() - lastColonBlink > 500) {
    colonState = !colonState;
    lastColonBlink = millis();
  }
  
  int hours24 = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();
  
  int hours12 = hours24 % 12;
  if (hours12 == 0)
    hours12 = 12;
  
  int timeNumber = hours12 * 100 + minutes;
  
  // Handle LCD and segment display blinking during alarm
  if (alarmRinging) {
    // Blink segment display every 400ms
    if (millis() - lastSegmentBlink > 400) {
      segmentDisplayOn = !segmentDisplayOn;
      lastSegmentBlink = millis();
      
      if (segmentDisplayOn) {
        segDisplay.setBrightness(7);
        segDisplay.showNumberDecEx(timeNumber, colonState ? 0b01000000 : 0, true);
      } else {
        segDisplay.setBrightness(0);  // Turn off display
      }
    }
    
    // Blink LCD backlight every 500ms (slower for better visibility)
    if (millis() - lastLcdBlink > 500) {
      lastLcdBlink = millis();
      
      // Toggle backlight
      backlightState = !backlightState;
      
      if (backlightState) {
        lcd.backlight();
        Serial.println("LCD ON");  // Debug to see if it's toggling
      } else {
        lcd.noBacklight();
        Serial.println("LCD OFF"); // Debug to see if it's toggling
      }
    }
    
    // If display is on, update time (in case it changed)
    if (segmentDisplayOn) {
      segDisplay.showNumberDecEx(timeNumber, colonState ? 0b01000000 : 0, true);
    }
  } else {
    // Normal display without blinking
    segDisplay.showNumberDecEx(timeNumber, colonState ? 0b01000000 : 0, true);
  }
  
  // Alarm trigger check
  static int lastCheckedSecond = -1;
  if (seconds != lastCheckedSecond) {
    lastCheckedSecond = seconds;
    
    if (alarmEnabled && !alarmRinging && hours24 == alarmHour && minutes == alarmMinute) {
      Serial.println("*** TIME MATCH! TRIGGERING ALARM! ***");
      startAlarm();
    }
  }
  
  // Display scrolling message
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 300) {
    lastUpdate = millis();
    
    if (factText.length() <= 32) {
      String line1 = factText.substring(0, min(16, (int)factText.length()));
      String line2 = "";
      
      if (factText.length() > 16) {
        line2 = factText.substring(16);
      }
      
      updateLCD(line1, line2);
    }
    else {
      String text = factText + "                                ";
      if (scrollPos >= text.length())
        scrollPos = 0;
      
      String visible = text.substring(
        scrollPos,
        min(scrollPos + 32, (int)text.length())
      );
      
      while (visible.length() < 32)
        visible += " ";
      
      updateLCD(visible.substring(0, 16), visible.substring(16, 32));
      scrollPos++;
    }
  }
  
  // Ring alarm with EXACT pattern from your test code
  if (alarmRinging) {
    unsigned long currentTime = millis();
    
    switch(alarmPatternStep) {
      case 0: // First beep
        beep(300);
        alarmPatternStep = 1;
        lastAlarmPatternTime = millis();
        break;
        
      case 1: // First delay
        if (currentTime - lastAlarmPatternTime >= 100) {
          alarmPatternStep = 2;
          lastAlarmPatternTime = millis();
        }
        break;
        
      case 2: // Second beep
        beep(300);
        alarmPatternStep = 3;
        lastAlarmPatternTime = millis();
        break;
        
      case 3: // Second delay
        if (currentTime - lastAlarmPatternTime >= 100) {
          alarmPatternStep = 4;
          lastAlarmPatternTime = millis();
        }
        break;
        
      case 4: // Third beep
        beep(300);
        alarmPatternStep = 5;
        lastAlarmPatternTime = millis();
        break;
        
      case 5: // Long delay before repeating
        if (currentTime - lastAlarmPatternTime >= 1000) {
          alarmPatternStep = 0; // Repeat pattern
          lastAlarmPatternTime = millis();
        }
        break;
    }
  }
  
  delay(1);
}