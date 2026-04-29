/*
 * =============================================================
 *  PROJECT : Speech Recognition System — Command-Based Device Control
 *  MODULE  : Arduino UNO + Elechouse Voice Recognition Module V3
 *  AUTHOR  : Embedded Systems Internship — Task 4
 *  DATE    : 2026
 *  REF     : https://quartzcomponents.com/blogs/electronics-projects/
 *            controlling-devices-using-voice-recognition-module-and-arduino
 * =============================================================
 *
 *  HARDWARE CONNECTIONS:
 *  ┌──────────────────────────────────────────────────────────┐
 *  │  Voice Module Pin │  Arduino UNO Pin                     │
 *  │──────────────────────────────────────────────────────────│
 *  │  VCC              │  5V                                  │
 *  │  GND              │  GND                                 │
 *  │  TX               │  D10 (SoftwareSerial RX)            │
 *  │  RX               │  D11 (SoftwareSerial TX)            │
 *  ├──────────────────────────────────────────────────────────┤
 *  │  LED / Relay      │  D13 (built-in LED or relay signal) │
 *  │  LED GND          │  GND (via 220Ω resistor)            │
 *  └──────────────────────────────────────────────────────────┘
 *
 *  TRAINED COMMAND MAPPING (stored in Voice Module slots 0–4):
 *  ┌────────────────────────────────────────────────────────┐
 *  │  Slot │  Voice Command   │  Action                    │
 *  │───────────────────────────────────────────────────────│
 *  │   0   │  "Light ON"      │  LED / Relay → HIGH (ON)  │
 *  │   1   │  "Light OFF"     │  LED / Relay → LOW (OFF)  │
 *  │   2   │  "Fan ON"        │  Fan Relay   → HIGH       │
 *  │   3   │  "Fan OFF"       │  Fan Relay   → LOW        │
 *  │   4   │  "All OFF"       │  All outputs → LOW        │
 *  └────────────────────────────────────────────────────────┘
 *
 *  DEPENDENCIES:
 *  - Elechouse VoiceRecognitionV3 library
 *    (https://github.com/elechouse/VoiceRecognitionV3)
 *  - SoftwareSerial (built-in with Arduino IDE)
 * =============================================================
 */

#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

/* ── SoftwareSerial port: RX=D10, TX=D11 ────────────────── */
VR myVR(10, 11);

/* ── Output pin definitions ─────────────────────────────── */
#define LIGHT_PIN   13    // Built-in LED or relay for "Light"
#define FAN_PIN     12    // Second relay for "Fan"
#define STATUS_LED   8    // Optional status indicator LED

/* ── Voice command slot indices (must match training) ────── */
#define CMD_LIGHT_ON   0
#define CMD_LIGHT_OFF  1
#define CMD_FAN_ON     2
#define CMD_FAN_OFF    3
#define CMD_ALL_OFF    4

/* ── Recognition result buffer ──────────────────────────── */
uint8_t buf[64];

/* ────────────────────────────────────────────────────────── */
/*  setup()                                                  */
/* ────────────────────────────────────────────────────────── */
void setup() {
  /* Start hardware serial for debug output to Serial Monitor */
  Serial.begin(115200);
  Serial.println(F("=============================================="));
  Serial.println(F("  Voice Recognition System — Arduino UNO     "));
  Serial.println(F("  Task 4 | Elechouse VR Module V3            "));
  Serial.println(F("=============================================="));

  /* Initialise output pins */
  pinMode(LIGHT_PIN,  OUTPUT);
  pinMode(FAN_PIN,    OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  /* Make sure all outputs start LOW (safe state) */
  digitalWrite(LIGHT_PIN,  LOW);
  digitalWrite(FAN_PIN,    LOW);
  digitalWrite(STATUS_LED, LOW);

  /* Initialise voice recognition module at 9600 baud */
  myVR.begin(9600);

  /* Brief blink to indicate power-on */
  blinkStatus(3);

  /* Load the five trained command records into the recogniser */
  Serial.println(F("Loading trained voice commands..."));

  if (myVR.load((uint8_t)CMD_LIGHT_ON)  >= 0) Serial.println(F("  [OK] Slot 0 : Light ON"));
  if (myVR.load((uint8_t)CMD_LIGHT_OFF) >= 0) Serial.println(F("  [OK] Slot 1 : Light OFF"));
  if (myVR.load((uint8_t)CMD_FAN_ON)    >= 0) Serial.println(F("  [OK] Slot 2 : Fan ON"));
  if (myVR.load((uint8_t)CMD_FAN_OFF)   >= 0) Serial.println(F("  [OK] Slot 3 : Fan OFF"));
  if (myVR.load((uint8_t)CMD_ALL_OFF)   >= 0) Serial.println(F("  [OK] Slot 4 : All OFF"));

  Serial.println(F("\nSystem ready. Speak a command!"));
  Serial.println(F("----------------------------------------------"));
}

/* ────────────────────────────────────────────────────────── */
/*  loop()                                                   */
/* ────────────────────────────────────────────────────────── */
void loop() {
  /*
   *  myVR.recognize() is non-blocking.
   *  Returns number of bytes in result buffer, or -1 if nothing recognised.
   *  buf[1] holds the slot index of the matched command.
   */
  int ret = myVR.recognize(buf, 50);

  if (ret > 0) {
    /* A command was recognised — dispatch to handler */
    handleCommand(buf[1]);
  }
  /* No action needed when ret == -1 (still listening) */
}

/* ────────────────────────────────────────────────────────── */
/*  handleCommand()                                          */
/*  Maps recognised slot ID → hardware action               */
/* ────────────────────────────────────────────────────────── */
void handleCommand(uint8_t commandID) {
  /* Flash status LED once to acknowledge recognition */
  blinkStatus(1);

  switch (commandID) {

    /* ── "Light ON" ──────────────────────────────────────── */
    case CMD_LIGHT_ON:
      digitalWrite(LIGHT_PIN, HIGH);
      Serial.println(F("[ACTION] Light ON  → Pin 13 = HIGH"));
      break;

    /* ── "Light OFF" ─────────────────────────────────────── */
    case CMD_LIGHT_OFF:
      digitalWrite(LIGHT_PIN, LOW);
      Serial.println(F("[ACTION] Light OFF → Pin 13 = LOW"));
      break;

    /* ── "Fan ON" ────────────────────────────────────────── */
    case CMD_FAN_ON:
      digitalWrite(FAN_PIN, HIGH);
      Serial.println(F("[ACTION] Fan ON    → Pin 12 = HIGH"));
      break;

    /* ── "Fan OFF" ───────────────────────────────────────── */
    case CMD_FAN_OFF:
      digitalWrite(FAN_PIN, LOW);
      Serial.println(F("[ACTION] Fan OFF   → Pin 12 = LOW"));
      break;

    /* ── "All OFF" ───────────────────────────────────────── */
    case CMD_ALL_OFF:
      digitalWrite(LIGHT_PIN, LOW);
      digitalWrite(FAN_PIN,   LOW);
      Serial.println(F("[ACTION] ALL OFF   → All pins LOW"));
      break;

    /* ── Unrecognised slot (safety fallback) ─────────────── */
    default:
      Serial.print(F("[WARN]  Unknown command slot: "));
      Serial.println(commandID, DEC);
      break;
  }

  Serial.println(F("----------------------------------------------"));
}

/* ────────────────────────────────────────────────────────── */
/*  blinkStatus()                                            */
/*  Blinks the status LED n times at 150 ms on/off rate     */
/* ────────────────────────────────────────────────────────── */
void blinkStatus(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(150);
    digitalWrite(STATUS_LED, LOW);
    delay(150);
  }
}
