/* -*- C++ -*- */
/********************
May 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

www.r-site.net

using plugins, in this example an alternative field that responds to
escape by restoring the original value and terminating edit.

all sorts of plugins may be available in the future and if you customize
a component and think it of interest of others please do pull request.

contrubutions and contrubutors are welcome.

using a plugin:
***/

#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>
#include <plugin/cancelField.h>

using namespace Menu;

int test=55;

//a menu using a plugin field
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,altFIELD(cancelField,test,"Cancelable edit","%",0,100,10,1,doNothing,enterEvent,wrapStyle)
  ,FIELD(test,"Original edit","%",0,100,10,1,doNothing,noEvent,wrapStyle)
);

#define MAX_DEPTH 1

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  options->numValueInput=false;//numeric keys in fields used as aceeletors instead
  Serial<<"menu 3.x plugins"<<endl;Serial.flush();
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
