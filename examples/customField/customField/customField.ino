#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
// #include <Streaming.h>

using namespace Menu;

int test=55;

//custom field edit
//implementing a customized menu component
// --this components can be latter shared as plugins
// is already stored as a plugin
template<typename T>
class cancelField:public menuField<T> {
protected:
  T original;//to use when canceling
  bool editing;
public:
  cancelField(constMEM menuFieldShadow<T>& shadow):menuField<T>(shadow),editing(false) {}
  void doNav(navNode& nav,navCmd cmd) override {
    if (!editing) {
      original=menuField<T>::target();
      editing=true;
    }
    switch(cmd.cmd) {
      case escCmd:
        editing=false;
        menuField<T>::target()=original;
        menuField<T>::tunning=true;
        break;
      case enterCmd:
        if (menuField<T>::tunning||nav.root->nav2D||!menuField<T>::tune())
          editing=false;
        break;
      default: break;
    }
    menuField<T>::doNav(nav,cmd);
  }
};

//a menu using a customized components
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,altFIELD(cancelField,test,"Custom edit","%",0,100,10,1,doNothing,enterEvent,wrapStyle)
  ,FIELD(test,"Original edit","%",0,100,10,1,doNothing,noEvent,wrapStyle)
);

#define MAX_DEPTH 1

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x testing custom fields");
  Serial.flush();
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
