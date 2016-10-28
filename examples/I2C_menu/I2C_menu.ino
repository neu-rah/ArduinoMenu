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
#include <LiquidCrystal_I2C.h>//F. Malpartida LCD's driver
#include <menu.h>//menu macros and objects
#include <menuLCDs.h>
#include <menuFields.h>
#include <quadEncoder.h>//quadrature encoder driver and fake stream
#include <keyStream.h>//keyboard driver and fake stream (for the encoder button)
#include <chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)

// rotary encoder pins
#define encA    2
#define encB    3
#define encBtn  4

#define LEDPIN 13

//LiquidCrystal_I2C lcd(0x27);//, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address and pinout

//aux vars
int ledCtrl=0;
bool runMenu=false;
bool scrSaverEnter=true;
int percent;//just testing changing this var
double fps=0;
unsigned long lastFpsChk=0;
int counter=0;

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions
bool pauseMenu() {
  runMenu=false;
  scrSaverEnter=true;
}
bool ledOn() {
  Serial.println("set led on!");
  digitalWrite(LEDPIN,ledCtrl=1);
  return false;
}

bool ledOff() {
  Serial.println("set led off!");
  digitalWrite(LEDPIN,ledCtrl=0);
  return false;
}

bool quit() {
  Serial.println("Quiting after action call");
  return true;
}

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
// empty options are just for scroll testing

/*bool setLed() {
  digitalWrite(LEDPIN,ledCtrl);
  return false;
}*/
TOGGLE(ledCtrl,setLed,"Led:",
    VALUE("On",HIGH,ledOn),
    VALUE("Off",LOW,ledOff)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",
  VALUE("Zero",0),
  VALUE("One",1),
  VALUE("Two",2)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose ",
  VALUE("First",1),
  VALUE("Second",2),
  VALUE("Third",3),
  VALUE("Last",-1)
);

MENU(subMenu,"SubMenu"
  ,OP("A",quit)
  ,OP("B",quit)
  ,OP("C",quit)
  ,OP("D",quit)
  ,OP("E",quit)
  ,OP("F",quit)
  ,OP("G",quit)
  ,OP("H",quit)
);

MENU(mainMenu,"Main menu",
  SUBMENU(setLed),
  OP("LED On",ledOn),
  OP("LED Off",ledOff),
  SUBMENU(selMenu),
  SUBMENU(chooseMenu),
  SUBMENU(subMenu),
  FIELD(percent,"Percent","%",0,100,10,1),
  FIELD(fps,"fps [","]",0,0,0,0),
  FIELD(counter,"counter [","]",0,0,0,0),
  OP("Exit",pauseMenu)
);

void scrSaver() {
  if (scrSaverEnter) {
    lcd.clear();
    lcd.print("|www.r-site.net|");
    lcd.setCursor(0,1);
    lcd.print("|click to enter|");
    scrSaverEnter=false;
  }
}

//the quadEncoder
#define ENC_SENSIVITY 4
quadEncoder quadEncoder(encA,encB);//simple quad encoder driver
quadEncoderStream enc(quadEncoder,ENC_SENSIVITY);// simple quad encoder fake Stream

//a keyboard with only one key :D, this is the encoder button
keyMap encBtn_map[]={{-encBtn,menu::enterCode}};//negative pin numbers means we have a pull-up, this is on when low
keyLook<1> encButton(encBtn_map);

//alternative to previous but now we can input from Serial too...
Stream* in3[]={&enc,&encButton,&Serial};
chainStream<3> allIn(in3);

//describing a menu output, alternatives so far are Serial or LiquidCrystal LCD
menuLCD menu_lcd(lcd,16,2);//menu output device

void setup()
{
  pinMode(LEDPIN,OUTPUT);
  pinMode(encBtn,INPUT_PULLUP);
  Serial.begin(115200);
  while(!Serial);
  Wire.begin();
  quadEncoder.begin();
  lcd.begin(16,2);
  lcd.home();
  lcd.print("Menu 2.x test");
}

void loop()
{
  if (runMenu) mainMenu.poll(menu_lcd,allIn);
  else if (allIn.read()==menu::enterCode) runMenu=true;
  else scrSaver();
  //simulate the delay of your program... if this number rises too much the menu will have bad navigation experience
  //if so, then the menu can be wired into a timmer... leaving the shorter end to your code while it is running
  counter=millis()/1000%60;
  int d=micros()-lastFpsChk;
  if (d>0) {
    fps=1000000.0/d;
    lastFpsChk+=d;
  }
  delay(50);
}
