#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>

#include <Streaming.h>

using namespace Menu;

int test=55;

//custom field edit
//implementing a customized menu component
//this components can be latter shared as plugins
template<typename T>
class cancelField:public menuField<T> {
protected:
  T original;//to use when canceling
  bool editing;
public:
  cancelField(const menuFieldShadow<T>& shadow):menuField<T>(shadow),editing(false) {}
  void doNav(navNode& nav,navCmd cmd) override {
    if (!editing) {
      original=target();
      editing=true;
    }
    switch(cmd.cmd) {
      case escCmd:
        editing=false;
        target()=original;
        menuField<T>::tunning=true;
        break;
      case enterCmd:
        if (menuField<T>::tunning||options->nav2D||!tune())
          editing=false;
        break;
      default: break;
    }
    menuField<T>::doNav(nav,cmd);
  }
};

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

//a menu using a customized components
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  //,altFIELD(cancelField,test,"Custom edit","%",0,100,10,1,doNothing,enterEvent,wrapStyle)
  //,FIELD(test,"Original edit","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,OP("OP1",doNothing,noEvent)
  ,OP("OP2",doNothing,noEvent)
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
  Serial<<"menu 3.0 testing custom fields"<<endl;Serial.flush();
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
