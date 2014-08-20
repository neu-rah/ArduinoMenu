#include <Wire.h>
#include <VPinsI2C.h>
#include <virtual_pins.h>
#include <HardwareSerial.h>
#include <LiquidCrystal.h>
#include "pcint.h"//this is incompatible with software serial (arduino needs an handler!)
#include "menu.h"//menu macros and objects
#include "quadEncoder.h"//quadrature encoder driver and fake stream
#include "keyStream.h"//keyboard driver and fake stream (for the encoder button)
#include "chainStream.h"// concatenate multiple input streams (this allows adding a button to the encoder)
#include "menuLCD.h"
#include "menuPrint.h"

//#define LCD_SZ_X 16
//#define LCD_SZ_Y 2

//how the LCS was wired
//#define LCD_WIRE NONE
#define LCD_WIRE VPINS_I2C
//#define LCD_WIRE VPINS_SPI//not implemented
//#define LCD_WIRE I2C//not implemented
//#define LCD_WIRE DIRECT//not implemented

#if LCD_WIRE==VPINS_I2C// LCD is wired over i2c to another AVR that uses virtual pins and shift registers to control it and virtual port server to share it
  //remote pins over I2C to VPort server
  #define vpinsSPI_CS srv_pb.pin(1)//vpins SPI CS 
  #define STCP srv_pb.pin(1)//stcp or latch pin
  
  //setup i2c network and port/pin/device maps
  // my LCD is remote wired... other LCD's should initialize as usual
  I2CServerBranch srv_vpa(Wire,0x11,VPA,VPA,1);//virtual (spi) port on i2c vport server (2 bytes)
  I2CServerBranch srv_pb(Wire,0x11,VPB,PB,1);//physical port B on i2c vport server is now local virtual port VPB (tied)
  I2CServerBranch srv_pc(Wire,0x11,VPC,PC,1);//physical port C on i2c vport server is now local virtual port VPC (tied)
  I2CServerBranch srv_pd(Wire,0x11,VPD,PD,1);//physical port D on i2c vport server is now local virtual port VPD (tied)
  //here wiring the LCD screen and other stuff over the I2C network...
  LiquidCrystal lcd1(srv_vpa.pin(6), srv_vpa.pin(4), srv_vpa.pin(0), srv_vpa.pin(1), srv_vpa.pin(2), srv_vpa.pin(3));
#endif

////////////////////////////////////////////
// ENCODER (aka rotary switch) PINS
// rotary
#define encA 2
#define encB 4
//this encoder has a button here
#define encBtn A0

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions

//aux function
void setValue(prompt &p,menuOut &o, Stream &i,int &value,const char* text,const char* units="",int sensivity=5,int low=0,int hi=100,int steps=0,void (*func)()=nothing);

void ledOn() {digitalWrite(13,1);}
void ledOff() {digitalWrite(13,0);}

int frequency=100;
void setFreq(prompt &p,menuOut &o,Stream &i) {setValue(p,o,i,frequency,"Freq:","0 Hz",20,1,1000);}

int dutty=50;
void setDutty(prompt &p,menuOut &o,Stream &i) {setValue(p,o,i,dutty,"Dutty:","%",1,0,100);}

void completeHandlerTest(prompt &p,menuOut &o,Stream &i) {
  o.clear();
  o.print("Handler test ok!");
  while(i.read()!=13);
}

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
MENU(subMenu,"LED on pin 13",
  OP("LED On",ledOn),
  OP("LED Off",ledOff)
);

MENU(mainMenu,"Sistema",
  OP("Frequency",setFreq),
  OP("Dutty",setDutty),
  OP("Handler test",completeHandlerTest),
  SUBMENU(subMenu)
);

//the quadEncoder
quadEncoder quadEncoder(encA,encB);//simple quad encoder driver
quadEncoderStream enc(quadEncoder,5);// simple quad encoder fake Stream

//a keyboard with only one key :D, this is the encoder button
keyMap encBtn_map[]={{-encBtn,13}};//negative pin numbers means we have a pull-up, this is on when low
keyLook<1> encButton(encBtn_map);

//multiple inputs allow conjugation of the quadEncoder with a single key keyboard that is the quadEncoder button
Stream* in[]={&enc,&encButton};
chainStream<2> quadEncoder_button(in);

//alternative to previous but now we can input from Serial too...
Stream* in3[]={&enc,&encButton,&Serial};
chainStream<3> allIn(in3);

//describing a menu output, alternatives so far are Serial or LiquidCrystal LCD
menuPrint serial(Serial);
menuLCD lcd(lcd1,16,2);

//menuOut* out[]={&lcd,&serial};
//chainOut<2> allOut(out);

///////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Serial.println("menu system test");
  Wire.begin();

  // LCD is wired over i2c to another AVR that uses virtual pins and shift registers to control it and virtual port server to share it  Serial.println("waiting for servers...");
  srv_vpa.begin();// wait for server ready (all on same server, so)
  //srv_vpb.begin();// wait for server ready
  Serial.println("all servers ready!");

  lcd1.begin(16,2);
  lcd1.print("Menu test");

  pinMode(encBtn, INPUT);
  digitalWrite(encBtn,1);

  pinMode(13,OUTPUT);
}

///////////////////////////////////////////////////////////////////////////////
// testing the menu system
void loop() {
  //mainMenu.activate(Serial,Serial);//show menu to Serial and read keys from Serial
  mainMenu.activate(lcd,allIn);//show menu on LCD and use multiple inputs to navigate (defined encoder, encoder button, serial)
  //mainMenu.activate(lcd1,allIn);//show menu on LCD and multiple inputs to navigate, defaults to LCD 16x1
  //mainMenu.activate(lcd,Serial);//very bad combination!
  //mainMenu.activate(Serial,enc);//bad combination! shopw menu on serial and navigate using quadEncoder
}

void nothing() {}

void percentBar(menuOut &o,int percent) {
  int i=map(percent, 0, 100, 0, o.maxX);
  for(int n=0;n<o.maxX;n++)
     o.print((char)(n<i?255:' '));
}

//read a value from the input stream device (encoder or serial)
void setValue(prompt &p,menuOut &o, Stream &i,int &value,const char* text,const char* units,int sensivity,int low,int hi,int steps,void (*func)()) {
  o.clear();
  int at=strlen(text);//.length();
  o.setCursor(0,0);
  o.print(text);
  if (o.style==menuOut::enumerated) {//probably a Serial terminal -------------------------------------
    //long timeout because some terminals send data righ away when typed, and parseInt would parse a partial number
    i.setTimeout(10000);//lib gives no access to previous timeout value :( ... cant restore it then, i would wait forever if possible
    value=i.parseInt();//assuming data was all delivered to the buffer (we had a large timeout)
    //clamp the entry
    if (value>hi) value=hi;
    else if (value<low) value=low;
    i.setTimeout(1000);//assuming it was default
    Serial.println(value);//feed back
    while(i.available()) i.read();//clean up extra characters to avoid reentry
  } else {// then we assume its some kind of encoder ---------------------------------------------------
    if (!steps) steps=(hi-low)/(float)o.maxX;
    float fact=((float)sensivity)/((float)steps);//sensivity factor
    float pos=quadEncoder.pos*fact;
    float last=pos;
    while(encButton.read()!=13) {
      //clamp value
      if (value>hi) value=hi;
      else if (value<low) value=low;
      o.setCursor(at,0);
      o.print(value);
      o.print(units);
      o.print("  ");
      o.setCursor(0,1);
      percentBar(o,map(value,low,hi,0,100));
      pos=quadEncoder.pos*fact;
      int delta=pos-last;
      if (delta) {
        value+=delta;
        last=pos;
      }
      //func();
    }
    delay(100);
    while(encButton.read()==13);
  }
}

