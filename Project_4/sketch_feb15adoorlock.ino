#include <ESP8266WiFi.h>
#include <espnow.h>

// ============== RELAYS ==============
#define RELAY_LOCK   D1
#define RELAY_UNLOCK D2

// ============== BUTTONS =============
#define BTN_LOCK     D5
#define BTN_UNLOCK   D6

// ============== TIMING ==============
#define MOTOR_TIME 7000
#define RELAY_GAP  200

// ============== STATE ===============
enum DoorState {
  LOCKED,
  UNLOCKED
};

DoorState doorState = UNLOCKED;

// ========== MOTOR CONTROL ==========
unsigned long motorStartTime = 0;
bool motorRunning = false;

// ========== BUTTON STATE ==========
bool lastLockState = HIGH;
bool lastUnlockState = HIGH;

// ========== ESP-NOW MESSAGE ==========
typedef struct struct_message {
  int command;   // 1 = LOCK, 2 = UNLOCK
} struct_message;

// ============== FUNCTIONS ==============

void allOff() {
  digitalWrite(RELAY_LOCK, HIGH);
  digitalWrite(RELAY_UNLOCK, HIGH);
}

void lockDoor() {
  if (doorState == LOCKED || motorRunning) return;

  Serial.println("Locking...");
  allOff();
  delay(RELAY_GAP);

  digitalWrite(RELAY_LOCK, LOW);
  motorStartTime = millis();
  motorRunning = true;

  doorState = LOCKED;
}

void unlockDoor() {
  if (doorState == UNLOCKED || motorRunning) return;

  Serial.println("Unlocking...");
  allOff();
  delay(RELAY_GAP);

  digitalWrite(RELAY_UNLOCK, LOW);
  motorStartTime = millis();
  motorRunning = true;

  doorState = UNLOCKED;
}

// ========== ESP-NOW RECEIVE ==========
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  struct_message msg;
  memcpy(&msg, incomingData, sizeof(msg));

  Serial.print("ESP-NOW Command: ");
  Serial.println(msg.command);

  if (msg.command == 1) lockDoor();
  if (msg.command == 2) unlockDoor();
}

// ========== BUTTON CHECK ==========
void checkButtons() {

  bool lockState = digitalRead(BTN_LOCK);
  bool unlockState = digitalRead(BTN_UNLOCK);

  // LOCK button pressed
  if (lastLockState == HIGH && lockState == LOW) {
    Serial.println("Manual LOCK");
    lockDoor();
    delay(200);
  }

  // UNLOCK button pressed
  if (lastUnlockState == HIGH && unlockState == LOW) {
    Serial.println("Manual UNLOCK");
    unlockDoor();
    delay(200);
  }

  lastLockState = lockState;
  lastUnlockState = unlockState;
}

// ============== SETUP ==============
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_LOCK, OUTPUT);
  pinMode(RELAY_UNLOCK, OUTPUT);

  pinMode(BTN_LOCK, INPUT_PULLUP);
  pinMode(BTN_UNLOCK, INPUT_PULLUP);

  allOff();

  WiFi.mode(WIFI_STA);
  wifi_set_channel(1);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Door System Ready (ESP-NOW + Buttons)");
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
}

// ============== LOOP ==============
void loop() {

  // Manual buttons
  checkButtons();

  // Motor auto stop
  if (motorRunning && millis() - motorStartTime >= MOTOR_TIME) {
    allOff();
    motorRunning = false;
    Serial.println("Motor stopped");
  }
}