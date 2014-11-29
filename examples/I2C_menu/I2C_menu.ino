#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <menu.h>//menu macros and objects
#include <menuLCDs.h>//F. Malpartida LCD's

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
menuLCD menu_lcd(lcd,16,2);//menu output device

void nothing() {}

MENU(mainMenu,"Main",
  OP("Frequency",nothing),
  OP("Dutty",nothing),
  OP("Setup",nothing)
);
  
void setup()
{
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16,2);
  lcd.print("Ok");  
}

void loop()
{
  //serial can use numbers or +/- and Enter to navigate
  mainMenu.poll(menu_lcd,Serial);
}
