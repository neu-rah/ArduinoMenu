/********************
Arduino generic menu system
LCD menu - unsing arduino classic LCD library
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Sept.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

*/

//#define DEBUG

#include <HardwareSerial.h>
#include <LiquidCrystal.h>
#include <menu.h>//menu macros and objects
#include <pcint.h>//this is incompatible with software serial (arduino needs an handler!)
#include <quadEncoder.h>//quadrature encoder driver and fake stream
#include <keyStream.h>//keyboard driver and fake stream (for the encoder button)
#include <chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include <menuLCD.h>
#include <menuFields.h>

 #if defined(__AVR_ATmega2560__)
  //LCD wired on my AtMega2560
  #define RS 44
  #define RW 46
  #define EN 48
  LiquidCrystal lcd1(RS, RW, EN, 43, 45, 47, 49);
#elif defined (__AVR_ATmega328P__)
  #define RS 8
  #define RW 3
  #define EN 9
  LiquidCrystal lcd1(RS, RW, EN, 4, 5, 6, 7);
#else
  #error "DEFINE YOUR LCD WIRING HERE (search for this message on code)"
#endif

////////////////////////////////////////////
// ENCODER (aka rotary switch) PINS
// rotary
#if defined(__AVR_ATmega328__)||defined(__AVR_ATmega328P__)// uno and breaduinos
  #define encA 2
  #define encB 3
  //this encoder has a button here
  #define encBtn 4
  #define LEDPIN 13
#elif defined(__AVR_ATmega2560__)
  #define encA 10
  #define encB 12
  #define encBtn 13
  #define LEDPIN 5
#endif

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
TOGGLE(ledCtrl,setLed,"Led: ",
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
    lcd1.clear();
    lcd1.print("|www.r-site.net|");
    lcd1.setCursor(0,1);
    lcd1.print("|click to enter|");
    scrSaverEnter=false;
  }
}

//the quadEncoder
quadEncoder quadEncoder(encA,encB);//simple quad encoder driver
quadEncoderStream enc(quadEncoder,4);// simple quad encoder fake Stream

//a keyboard with only one key :D, this is the encoder button
keyMap encBtn_map[]={{-encBtn,menu::enterCode}};//negative pin numbers means we have a pull-up, this is on when low
keyLook<1> encButton(encBtn_map);

//multiple inputs allow conjugation of the quadEncoder with a single key keyboard that is the quadEncoder button
//Stream* in[]={&enc,&encButton};
//chainStream<2> quadEncoder_button(in);

//alternative to previous but now we can input from Serial too...
Stream* in3[]={&enc,&encButton,&Serial};
chainStream<3> allIn(in3);

//describing a menu output, alternatives so far are Serial or LiquidCrystal LCD
menuLCD lcd(lcd1,16,2);

/////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu system test");

  quadEncoder.begin();

  lcd1.begin(16,2);
  lcd1.print("Menu test");

  pinMode(encBtn, INPUT);
  digitalWrite(encBtn,1);

  pinMode(LEDPIN,OUTPUT);

  mainMenu[7].disable();
  mainMenu[8].disable();

  delay(300);
}

///////////////////////////////////////////////////////////////////////////////
// testing the menu system
void loop() {
  if (runMenu) mainMenu.poll(lcd,allIn);
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
