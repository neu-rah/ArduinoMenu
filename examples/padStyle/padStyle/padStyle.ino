#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>

#include <Streaming.h>

using namespace Menu;

/*
The pad style!

fmt: is for an output device, this is about a menu!
  => use a redirecting format (redirect draw to structures)
  => use a structure format handler
  => does not scale well!

plugins: they customize menu structures but menu structures do not receive draw requests!
  => make menu structures receive text requests, still someone has to do the print

flags: use current mechanism to consider extra pad case
  => just complicates current code
  => everything remains in place
  => does not favor extension
*/

altMENU(menu,sub,"Sub",doNothing,noEvent,wrapStyle,_asPad
  ,OP("Yes",doNothing,noEvent)
  ,OP("No",doNothing,noEvent)
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
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"menu 3.0 testing custom fields"<<endl;Serial.flush();
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
