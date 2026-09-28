#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

#include "HomeSpan.h"

// =====================================================
// WIFI
// =====================================================

const char* WIFI_SSID = "HARSH";
const char* WIFI_PASSWORD = "12345678";

// =====================================================
// ESP8266 #1 MAC ADDRESS
// =====================================================

uint8_t esp8266_1[] = {
  0x8C,
  0xAA,
  0xB5,
  0x75,
  0xFC,
  0x84
};

// =====================================================
// COMMAND STRUCTURE
// MUST MATCH ESP8266 RECEIVER
// =====================================================

struct Command {

  uint8_t device;

  char action[8];
};

// =====================================================
// ESP-NOW SEND CALLBACK
// ESP32 CORE 3.x / 3.3.x
// =====================================================

void onDataSent(
  const wifi_tx_info_t *info,
  esp_now_send_status_t status
) {

  Serial.print("ESP-NOW send status: ");

  if (status == ESP_NOW_SEND_SUCCESS) {

    Serial.println("SUCCESS");

  } else {

    Serial.println("FAILED");
  }
}

// =====================================================
// SEND COMMAND TO ESP8266
// =====================================================

void sendCommand(
  uint8_t device,
  const char* action
) {

  Command packet;

  packet.device = device;

  memset(
    packet.action,
    0,
    sizeof(packet.action)
  );

  strcpy(
    packet.action,
    action
  );


  Serial.println();
  Serial.println("------------------------------");

  Serial.print("Sending command: ");
  Serial.print(device);
  Serial.println(action);


  esp_err_t result = esp_now_send(
    esp8266_1,
    (uint8_t*)&packet,
    sizeof(packet)
  );


  if (result == ESP_OK) {

    Serial.println(
      "Packet accepted for sending"
    );

  } else {

    Serial.print(
      "ESP-NOW send error: "
    );

    Serial.println(result);
  }

  Serial.println("------------------------------");
}

// =====================================================
// HOMEKIT LIGHT
// =====================================================

struct MyLight : Service::LightBulb {

  SpanCharacteristic *power;


  MyLight() {

    power = new Characteristic::On();

    Serial.println(
      "HomeKit Light created"
    );
  }


  boolean update() override {

    bool isOn = power->getNewVal();


    Serial.print(
      "HomeKit command: "
    );


    if (isOn) {

      Serial.println("ON");

      sendCommand(
        1,
        "ON"
      );

    } else {

      Serial.println("OFF");

      sendCommand(
        1,
        "OFF"
      );
    }


    return true;
  }
};

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP32 HOMEKIT + ESP-NOW");
  Serial.println("==============================");

  // ============================================
  // WIFI
  // ============================================

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Wi-Fi channel: ");
  Serial.println(WiFi.channel());

  // ============================================
  // GET ACTUAL WIFI CHANNEL
  // ============================================

  uint8_t primary;
  wifi_second_chan_t second;

  esp_err_t channelResult =
    esp_wifi_get_channel(
      &primary,
      &second
    );

  if (channelResult == ESP_OK) {

    Serial.print("Actual ESP32 channel: ");
    Serial.println(primary);

  } else {

    Serial.println("Failed to read Wi-Fi channel");
  }

  // ============================================
  // ESP-NOW
  // ============================================

  if (esp_now_init() != ESP_OK) {

    Serial.println(
      "ESP-NOW initialization FAILED"
    );

    return;
  }

  Serial.println(
    "ESP-NOW initialized"
  );

  // ESP32 CORE 3.x CALLBACK
  esp_now_register_send_cb(
    onDataSent
  );

  // ============================================
  // ADD ESP8266 PEER
  // ============================================

  esp_now_peer_info_t peerInfo = {};

  memcpy(
    peerInfo.peer_addr,
    esp8266_1,
    6
  );

  // IMPORTANT:
  // Use the actual channel of the Wi-Fi connection
  peerInfo.channel = primary;

  peerInfo.encrypt = false;

  peerInfo.ifidx = WIFI_IF_STA;

  Serial.print(
    "ESP-NOW peer channel: "
  );

  Serial.println(
    peerInfo.channel
  );

  if (
    esp_now_add_peer(
      &peerInfo
    ) == ESP_OK
  ) {

    Serial.println(
      "ESP8266 #1 added successfully"
    );

  } else {

    Serial.println(
      "Failed to add ESP8266 #1"
    );
  }

  // ============================================
  // HOMESPAN
  // ============================================

  homeSpan.setPairingCode(
    "11122333"
  );

  homeSpan.begin(
    Category::Lighting,
    "ESP32 Home Controller"
  );

  // ============================================
  // HOMEKIT ACCESSORY
  // ============================================

  new SpanAccessory();

    new Service::AccessoryInformation();

      new Characteristic::Identify();

      new Characteristic::Name(
        "ESP32 Light"
      );

    new MyLight();

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP32 HOMEKIT + ESP-NOW READY");
  Serial.println("==============================");
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  homeSpan.poll();
}