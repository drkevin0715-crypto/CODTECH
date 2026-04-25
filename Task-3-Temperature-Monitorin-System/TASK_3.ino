#include <Adafruit_LiquidCrystal.h>  
// Includes library to control LCD using I2C (simplifies wiring)

int temp = 0;  
// Variable to store calculated temperature value

Adafruit_LiquidCrystal lcd_1(0);  
// Create LCD object (I2C address = 0 in Tinkercad default)

void setup()
{
  pinMode(A0, INPUT);  
  // Sets analog pin A0 as input (connected to TMP36 sensor)

  Serial.begin(9600);  
  // Starts serial communication at 9600 baud (for debugging/output)

  lcd_1.begin(16, 2);  
  // Initializes LCD with 16 columns and 2 rows
}

void loop()
{
  // Read analog value from TMP36 sensor (0–1023 range)
  int rawValue = analogRead(A0);

  // Convert analog reading to temperature using mapping
  // Breakdown:
  // (analogRead(A0) - 20) → small offset correction
  // * 3.04 → scaling factor (approx conversion to voltage/temp)
  temp = map(((rawValue - 20) * 3.04), 0, 1023, -20, 120);

  Serial.println(temp);  
  // Print temperature to Serial Monitor

  // Set cursor to first row, first column
  lcd_1.setCursor(0, 0);
  lcd_1.print("Temp. Monitr. system");  
  // Display system title

  // Set cursor to second row, first column
  lcd_1.setCursor(0, 1);
  lcd_1.print("Temp Value=");  
  // Display label

  // Move cursor to position where value will be shown
  lcd_1.setCursor(12, 1);
  lcd_1.print(temp);  
  // Print temperature value on LCD

  delay(10);  
  // Small delay to stabilize readings and avoid flickering
}