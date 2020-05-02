//tiny85 fuses for 16Mhz -U lfuse:w:0xf1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m
//8Mhz -U lfuse:w:0xe2:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m
//OSCCAL was at 93

// ***
// *** Pinout ATtiny25/45/85:
// *** PDIP/SOIC/TSSOP
// *** =============================================================================================
// ***
// ***        5/A0 (PCINT5/RESET/ADC0/dW) PB5   [1]*  [8]   VCC
// *** 3/A3 (PCINT3/XTAL1/CLKI/OC1B/ADC3) PB3   [2]   [7]   PB2 (SCK/USCK/SCL/ADC1/T0/INT0/PCINT2) 2/A1
// *** 4/A2 (PCINT4/XTAL2/CLKO/OC1B/ADC2) PB4   [3]   [6]   PB1 (MISO/DO/AIN1/OC0B/OC1A/PCINT1) 1
// ***                                    GND   [4]   [5]   PB0 (MOSI/DI/SDA/AIN0/OC0A/OC1A/AREF/PCINT0) 0
// ***

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  delay(5000);
  Serial.println("testing...");
}

void loop()
{
  if(Serial.available()) Serial.write(Serial.read());
  delay(1);
}
