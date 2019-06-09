////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu lcd+serial output chain example

// Just for comparing memory usage

// LCD /////////////////////////////////////////
#include <LiquidCrystal.h>
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("AM5 LiquidCrystal example"));
  lcd.begin(16,2);
}

void loop() {
}
