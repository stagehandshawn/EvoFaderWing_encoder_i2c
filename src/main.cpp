#include <Wire.h>
#include <Encoder.h>

// === Configuration ===
#ifndef ENCODER_GROUP
  #define ENCODER_GROUP 1
#endif

#define I2C_ADDRESS (0x10 + ENCODER_GROUP)
#define NUM_ENCODERS 5
#define SCALE_LEN 8
#define INT_PIN 8

// === Protocol Constants ===
#define DATA_TYPE_ENCODER  0x01
#define DATA_TYPE_KEYPRESS 0x02
#define DATA_TYPE_BUTTON   0x03

void sendEncoderData();
void handleEncoders();

const uint8_t startEncoder = ((ENCODER_GROUP - 1) * NUM_ENCODERS) + 1;

// Pins setup to be wired from top to bottom on the physical Atmega device
const int enc_pins[NUM_ENCODERS][2] = {
  {A3, A2},
  {A1, A0},
  {2, 3},
  {4, 5},
  {6, 7}
};

const int velocityScale[SCALE_LEN] = {1, 1, 2, 2, 3, 4, 5, 6};

Encoder* encoders[NUM_ENCODERS];
long encoderBuffer[NUM_ENCODERS] = {0};
long lastPos[NUM_ENCODERS] = {0};
unsigned long lastMoveTime[NUM_ENCODERS] = {0};

uint8_t encoderOut[NUM_ENCODERS] = {0};
bool encoderMoved = false;

void setup() {
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(sendEncoderData);

  pinMode(INT_PIN, OUTPUT);
  digitalWrite(INT_PIN, HIGH);

  Serial.begin(57600);
  Serial.println("[I2C SLAVE] Encoder slave starting...");
  Serial.print(" Encoders: ");
  Serial.print(startEncoder);
  Serial.print("-");
  Serial.println(startEncoder + NUM_ENCODERS - 1);
  Serial.print(" I2C Address: 0x");
  Serial.println(I2C_ADDRESS, HEX);

  #ifdef DEBUG
    Serial.print("Debug mode enabled");
  #else
    Serial.print("Debug mode disabled");
  #endif

  for (int i = 0; i < NUM_ENCODERS; i++) {
    encoders[i] = new Encoder(enc_pins[i][0], enc_pins[i][1]);
    lastPos[i] = encoders[i]->read();
  }
}

void loop() {
  handleEncoders();
  delay(1);
}

void handleEncoders() {
  unsigned long now = millis();

  for (int i = 0; i < NUM_ENCODERS; i++) {
    long movement = encoders[i]->readAndReset();
    encoderBuffer[i] += movement;

    while (abs(encoderBuffer[i]) >= 4) {
      int dir = (encoderBuffer[i] > 0) ? 1 : -1;
      encoderBuffer[i] -= (4 * dir);

      unsigned long elapsed = now - lastMoveTime[i];
      lastMoveTime[i] = now;

      int level = constrain((int)(elapsed / 15), 0, SCALE_LEN - 1);
      int scaled = velocityScale[SCALE_LEN - 1 - level];

      uint8_t val = (dir > 0) ? (0x80 | scaled) : scaled;

      encoderOut[i] = val;
      encoderMoved = true;

      #ifdef DEBUG
      Serial.print("[ENC]");
      Serial.print(" global_idx:"); Serial.print(startEncoder + i);
      Serial.print(" dir:"); Serial.print(dir);
      Serial.print(" step:"); Serial.print(scaled);
      Serial.print(" elapsed:"); Serial.print(elapsed);
      Serial.println();
      #endif
    }
  }

  if (encoderMoved) {
    digitalWrite(INT_PIN, LOW);
  }
}

void sendEncoderData() {
  #ifdef DEBUG
  Serial.print("[I2C] Sending encoder data...");
  #endif

  uint8_t activeEncoders = 0;
  for (int i = 0; i < NUM_ENCODERS; i++) {
    if (encoderOut[i] != 0) activeEncoders++;
  }

  Wire.write(DATA_TYPE_ENCODER);
  Wire.write(activeEncoders);

  for (int i = 0; i < NUM_ENCODERS; i++) {
    if (encoderOut[i] != 0) {
      uint8_t globalIndex = startEncoder + i;
      bool isPositive = (encoderOut[i] & 0x80) != 0;
      uint8_t velocity = encoderOut[i] & 0x7F;
      uint8_t indexWithDirection = globalIndex | (isPositive ? 0x80 : 0x00);

      Wire.write(indexWithDirection);
      Wire.write(velocity);

      #ifdef DEBUG
      Serial.print(" Enc"); Serial.print(globalIndex);
      Serial.print(isPositive ? "+" : "-");
      Serial.print("@"); Serial.print(velocity);
      #endif
    }
  }

  memset(encoderOut, 0, sizeof(encoderOut));
  encoderMoved = false;
  digitalWrite(INT_PIN, HIGH);

  #ifdef DEBUG
  Serial.println(" [SENT]");
  #endif
}