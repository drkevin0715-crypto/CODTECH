/*
 * ============================================================
 *   HOME AUTOMATION WITH BLUETOOTH
 *   Arduino Uno + HC-05 Bluetooth Module + Relay Module
 * ============================================================
 *   Author  : Internship Project
 *   Date    : 2026
 *   Version : 1.0
 *
 *   DESCRIPTION:
 *   This sketch receives single-character commands via Bluetooth
 *   (HC-05 module on Software Serial pins 10/11) and toggles
 *   up to 4 relay channels, each controlling an AC load.
 *
 *   COMMAND TABLE:
 *   '1' → Relay 1 ON   (Device 1 – Bulb/Light)
 *   '2' → Relay 2 ON   (Device 2 – Fan)
 *   '3' → Relay 3 ON   (Device 3 – Socket/Appliance)
 *   '4' → Relay 4 ON   (Device 4 – Spare)
 *   'a' → Relay 1 OFF
 *   'b' → Relay 2 OFF
 *   'c' → Relay 3 OFF
 *   'd' → Relay 4 OFF
 *   'A' → ALL Relays ON
 *   'X' → ALL Relays OFF
 *   'S' → Status report (which relays are ON)
 * ============================================================
 */

#include <SoftwareSerial.h>

// ── Pin Definitions ──────────────────────────────────────────
// HC-05: RX pin of module → D11 (TX of Arduino SW serial)
//         TX pin of module → D10 (RX of Arduino SW serial)
SoftwareSerial BTSerial(10, 11);  // (RX, TX) from Arduino's perspective

// Relay pins (active-LOW for most relay modules)
const int RELAY_1 = 2;   // Device 1 – Light / Bulb
const int RELAY_2 = 3;   // Device 2 – Fan
const int RELAY_3 = 4;   // Device 3 – Socket
const int RELAY_4 = 5;   // Device 4 – Spare

// Relay state tracking
bool relay1State = false;
bool relay2State = false;
bool relay3State = false;
bool relay4State = false;

// ── Helper: relay ON = send LOW (active-low module) ──────────
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println(F("Home Automation System Starting..."));

  BTSerial.begin(9600);

  pinMode(RELAY_1, OUTPUT); digitalWrite(RELAY_1, RELAY_OFF);
  pinMode(RELAY_2, OUTPUT); digitalWrite(RELAY_2, RELAY_OFF);
  pinMode(RELAY_3, OUTPUT); digitalWrite(RELAY_3, RELAY_OFF);
  pinMode(RELAY_4, OUTPUT); digitalWrite(RELAY_4, RELAY_OFF);

  Serial.println(F("Ready. Waiting for Bluetooth commands..."));
  BTSerial.println(F("HOME AUTOMATION READY"));
  BTSerial.println(F("Commands: 1-4=ON, a-d=OFF, A=ALL ON, X=ALL OFF, S=Status"));
}

// ── Main Loop ─────────────────────────────────────────────────
void loop() {
  if (BTSerial.available()) {
    char cmd = (char) BTSerial.read();
    processCommand(cmd);
  }

  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }
}

// ── Command Processor ─────────────────────────────────────────
void processCommand(char cmd) {
  Serial.print(F("Received command: "));
  Serial.println(cmd);

  switch (cmd) {

    case '1':
      setRelay(RELAY_1, true,  &relay1State, "Device 1 (Light)");   break;
    case '2':
      setRelay(RELAY_2, true,  &relay2State, "Device 2 (Fan)");     break;
    case '3':
      setRelay(RELAY_3, true,  &relay3State, "Device 3 (Socket)");  break;
    case '4':
      setRelay(RELAY_4, true,  &relay4State, "Device 4 (Spare)");   break;

    case 'a':
      setRelay(RELAY_1, false, &relay1State, "Device 1 (Light)");   break;
    case 'b':
      setRelay(RELAY_2, false, &relay2State, "Device 2 (Fan)");     break;
    case 'c':
      setRelay(RELAY_3, false, &relay3State, "Device 3 (Socket)");  break;
    case 'd':
      setRelay(RELAY_4, false, &relay4State, "Device 4 (Spare)");   break;

    case 'A':
      digitalWrite(RELAY_1, RELAY_ON);  relay1State = true;
      digitalWrite(RELAY_2, RELAY_ON);  relay2State = true;
      digitalWrite(RELAY_3, RELAY_ON);  relay3State = true;
      digitalWrite(RELAY_4, RELAY_ON);  relay4State = true;
      respond("ALL DEVICES ON");
      break;

    case 'X':
      digitalWrite(RELAY_1, RELAY_OFF); relay1State = false;
      digitalWrite(RELAY_2, RELAY_OFF); relay2State = false;
      digitalWrite(RELAY_3, RELAY_OFF); relay3State = false;
      digitalWrite(RELAY_4, RELAY_OFF); relay4State = false;
      respond("ALL DEVICES OFF");
      break;

    case 'S':
      sendStatus();
      break;

    default:
      respond("Unknown command. Send 1-4, a-d, A, X, or S.");
      break;
  }
}

// ── Set a single relay ────────────────────────────────────────
void setRelay(int pin, bool turnOn, bool* stateVar, const char* label) {
  *stateVar = turnOn;
  digitalWrite(pin, turnOn ? RELAY_ON : RELAY_OFF);
  String msg = String(label) + (turnOn ? " -> ON" : " -> OFF");
  respond(msg);
}

// ── Send text back to phone + Serial Monitor ──────────────────
void respond(String msg) {
  BTSerial.println(msg);
  Serial.println(msg);
}

// ── Send full device status ───────────────────────────────────
void sendStatus() {
  BTSerial.println(F("--- STATUS ---"));
  BTSerial.print(F("D1 Light : ")); BTSerial.println(relay1State ? "ON" : "OFF");
  BTSerial.print(F("D2 Fan   : ")); BTSerial.println(relay2State ? "ON" : "OFF");
  BTSerial.print(F("D3 Socket: ")); BTSerial.println(relay3State ? "ON" : "OFF");
  BTSerial.print(F("D4 Spare : ")); BTSerial.println(relay4State ? "ON" : "OFF");
  BTSerial.println(F("--------------"));

  Serial.println(F("--- STATUS ---"));
  Serial.print(F("D1 Light : ")); Serial.println(relay1State ? "ON" : "OFF");
  Serial.print(F("D2 Fan   : ")); Serial.println(relay2State ? "ON" : "OFF");
  Serial.print(F("D3 Socket: ")); Serial.println(relay3State ? "ON" : "OFF");
  Serial.print(F("D4 Spare : ")); Serial.println(relay4State ? "ON" : "OFF");
}
