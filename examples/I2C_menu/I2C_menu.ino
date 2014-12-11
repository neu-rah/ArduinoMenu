/********************
Arduino generic menu system
Arduino menu on I2C LCD example
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Sep.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no 
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

LCD library:
https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
http://playground.arduino.cc/Code/LCD3wires
*/
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
  //serial can use numbers or +/- and Enter=* Esc=/ to navigate (nav keys can be redefined)
  mainMenu.poll(menu_lcd,Serial);
}
