extern "C" {
  #include "user_interface.h"
}

#include <ESP8266WiFi.h>
#include <espnow.h>

// ============================================
// DEVICE
// ============================================

#define DEVICE_ID 1

// NodeMCU D1 = GPIO5
#define RELAY_PIN 5

// Most relay modules are ACTIVE LOW
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// ============================================
// COMMAND STRUCTURE
// MUST MATCH ESP32
// ============================================

struct Command {
  uint8_t device;
  char action[8];
};

Command receivedCommand;

// ============================================
// ESP-NOW RECEIVE
// ============================================

void onDataReceive(
  uint8_t *mac,
  uint8_t *data,
  uint8_t len
) {

  if (len != sizeof(Command)) {
    return;
  }

  memcpy(
    &receivedCommand,
    data,
    sizeof(receivedCommand)
  );

  Serial.print("Received command: ");
  Serial.println(receivedCommand.action);

  // Only respond to this device
  if (receivedCommand.device != DEVICE_ID) {
    return;
  }

  // ==========================================
  // LIGHT ON
  // ==========================================

  if (strcmp(receivedCommand.action, "ON") == 0) {

    digitalWrite(
      RELAY_PIN,
      RELAY_ON
    );

    Serial.println("Outdoor Light ON");
  }

  // ==========================================
  // LIGHT OFF
  // ==========================================

  else if (strcmp(receivedCommand.action, "OFF") == 0) {

    digitalWrite(
      RELAY_PIN,
      RELAY_OFF
    );

    Serial.println("Outdoor Light OFF");
  }
}

// ============================================
// SETUP
// ============================================

void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP8266 #1 OUTDOOR LIGHT");
  Serial.println("==============================");

  // ==========================================
  // RELAY
  // ==========================================

  pinMode(
    RELAY_PIN,
    OUTPUT
  );

  // Start with relay OFF
  digitalWrite(
    RELAY_PIN,
    RELAY_OFF
  );

  // ==========================================
  // WIFI
  // ==========================================

  WiFi.mode(WIFI_STA);

  // ESP-NOW must use channel 6
  wifi_set_channel(6);

  Serial.print("Channel: ");
  Serial.println(
    wifi_get_channel()
  );

  Serial.println();

  Serial.print("ESP8266 MAC: ");
  Serial.println(
    WiFi.macAddress()
  );

  // ==========================================
  // ESP-NOW
  // ==========================================

  if (esp_now_init() != 0) {

    Serial.println(
      "ESP-NOW initialization failed"
    );

    return;
  }

  esp_now_set_self_role(
    ESP_NOW_ROLE_SLAVE
  );

  esp_now_register_recv_cb(
    onDataReceive
  );

  Serial.println(
    "ESP-NOW initialized"
  );

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP8266 #1 READY");
  Serial.println("==============================");
}

// ============================================
// LOOP
// ============================================

void loop() {
}