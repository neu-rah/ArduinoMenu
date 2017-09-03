#include <Arduino.h>

#include <Wire.h>
// #include <LiquidCrystal_I2C.h>//F. Malpartida LCD's driver
#include <menu.h>
#include <menuIO/serialOut.h>
// #include <menuIO/lcdOut.h>//malpartidas lcd menu output

#include <Streaming.h>

using namespace Menu;

// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address and pinout

result justQuit() {
  Serial.println();
  Serial.println("default action for yes/no pad field");
  return quit;
}

altMENU(menu,sub,"Pad menu.",doNothing,noEvent,wrapStyle,_asPad
  ,OP("Yes",justQuit,enterEvent)
  ,OP("No",justQuit,enterEvent)
);

//a menu using a customized components
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  //,altFIELD(cancelField,test,"Custom edit","%",0,100,10,1,doNothing,enterEvent,wrapStyle)
  //,FIELD(test,"Original edit","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(sub)
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  // ,LCD_OUT(lcd,{0,0,20,4})
  ,SERIAL_OUT(Serial)
  ,NONE
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial<<"menu 3.0 testing pad style"<<endl;Serial.flush();
  // lcd.begin(20,4);
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
