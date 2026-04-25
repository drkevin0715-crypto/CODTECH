/**
 * ============================================================
 *  Push Button Triggered Temperature Display System
 * ============================================================
 *  Hardware  : Arduino UNO
 *  Sensor    : TMP36 Analog Temperature Sensor (A0)
 *  Input     : Push Button (Digital Pin 2, INPUT_PULLUP)
 *  Display   : 16x2 I2C LCD (Adafruit_LiquidCrystal, address 0)
 *
 *  Behavior  :
 *    - LCD shows "Press Button" on startup
 *    - When button is pressed (falling edge detected),
 *      the Arduino reads temperature from TMP36 and
 *      displays it on the LCD in degrees Celsius.
 *
 *  Author    : Embedded Systems Internship Project
 * ============================================================
 */

#include <Adafruit_LiquidCrystal.h>

// ── Pin Definitions ──────────────────────────────────────────
const int TEMP_PIN   = A0;  // TMP36 analog output → A0
const int BUTTON_PIN = 2;   // Push button → Digital Pin 2

// ── LCD Object (I2C address 0) ───────────────────────────────
Adafruit_LiquidCrystal lcd_1(0);

// ── Button State Tracking (for edge detection) ───────────────
int lastButtonState = HIGH;  // Starts HIGH because of INPUT_PULLUP

// ============================================================
//  setup()  –  Runs once on power-up / reset
// ============================================================
void setup() {
  // Configure button pin with internal pull-up resistor.
  // Pin reads HIGH normally; goes LOW when button is pressed.
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialise the 16-column × 2-row LCD
  lcd_1.begin(16, 2);

  // Show idle message until button is pressed
  lcd_1.print("Press Button");
}

// ============================================================
//  loop()  –  Runs repeatedly after setup()
// ============================================================
void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  // Detect a falling edge: button just transitioned HIGH → LOW
  // (i.e., the button was just pressed)
  if (lastButtonState == HIGH && buttonState == LOW) {
    readAndDisplayTemp();
  }

  // Save current state for comparison in the next iteration
  lastButtonState = buttonState;
}

// ============================================================
//  readAndDisplayTemp()
//  Reads the TMP36 sensor, converts to °C, updates the LCD
// ============================================================
void readAndDisplayTemp() {
  // Step 1 – Read the raw 10-bit ADC value (0–1023)
  int rawADC = analogRead(TEMP_PIN);

  // Step 2 – Convert ADC value to voltage
  //   Arduino ADC reference = 5.0 V, resolution = 1023 steps
  //   Voltage (V) = rawADC × (5.0 / 1023.0)
  float voltage = rawADC * (5.0 / 1023.0);

  // Step 3 – Convert voltage to temperature (°C)
  //   TMP36 outputs 10 mV per °C  →  temperature = voltage × 100
  //   (e.g., 0.75 V = 75 × 10 mV = 75 °C offset → actual = 25 °C
  //    with a 0 V offset at 0 °C using this simplified formula)
  float temperature = voltage * 100.0;

  // Step 4 – Update the LCD display
  lcd_1.clear();

  lcd_1.setCursor(0, 0);   // Row 0 (top)
  lcd_1.print("Temp:");

  lcd_1.setCursor(0, 1);   // Row 1 (bottom)
  lcd_1.print(temperature);
  lcd_1.print(" C");

  // Brief debounce delay to ignore switch bounce
  delay(300);
}
