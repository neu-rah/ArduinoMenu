/********************
Aug. 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

Self update custom field
output: Serial
input: Serial
www.r-site.net

***/
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

result myLedOn() {
  digitalWrite(LEDPIN, HIGH);
  return proceed;
}
result myLedOff() {
  digitalWrite(LEDPIN, LOW);
  return proceed;
}


//customizing a prompt look!
//by extending the prompt class
//this prompt will count seconds and update himself on the screen.
class altPrompt:public prompt {
public:
  unsigned int t=0;
  unsigned int last=0;
  altPrompt(constMEM promptShadow& p):prompt(p) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    last=t;
    return out.printRaw(String(t).c_str(),len);
  }
  virtual bool changed(const navNode &nav,const menuOut& out,bool sub=true) {
    t=millis()/1000;
    return last!=t;
  }
};

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,altOP(altPrompt,"",doNothing,noEvent)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

//when menu is suspended
//this function is called when entering or leaving suspended state
// with idleStart and idleend cases
//and at least once in between them (idling case)
//it might also be called for every pool (when in suspended state, idling case)
//for output devices that require refresh (repeated draw, idling case)
result idle(menuOut &o, idleEvent e) {
  o.clear();
  switch(e) {
    case idleStart:o.println("suspending menu!");break;
    case idling:o.println("suspended...");break;
    case idleEnd:o.println("resuming menu.");break;
  }
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,HIGH);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x test");Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended
  // nav.idleOn(idle);//enter idle mode, this menu will start on idle state, press select  to enter menu
  // nav.doInput("4");//we can also provide input from string, this will enter idle mode by exiting the menu

  //this is an example of driving the menu string input
  //this however requires a known menu state and therefor is not recomended for complicated menus
  // see async calls instead
  // for(int c=0;c<3;c++) {//using the menu to blink the led
  //   nav.doInput("2");//led on
  //   delay(1000);
  //   nav.doInput("3");//led off
  //   delay(1000);
  // }

  //NOTE: async is disabled now define MENU_ASYNC to activate it
  //following code is an async navigation example
  //its async because it does not require a known menu state
  //because you can indicate the zero indexed option positions separated by /
  //it also works for field by accepting the input
  //ex: "/0/1/12" -> will select first option on main menu,
  // then the second option (eventually a field)
  // and finally set the field value to 12 (or select 13th position on a menu)
  //
  // note that this is menu position starting at zero (not accel keys)
  // this is experimental API base for web interface
  // for(int c=0;c<3;c++) {//using the menu to blink the led
  //   nav.async("/1");
  //   delay(1000);
  //   nav.async("/2");
  //   delay(1000);
  // }

  //there is also the possibility of navigating the menu by calling api functions
  //this is however indicated if you have custom input devices
  //or building a driver for a new input.

  for(int c=0;c<10;c++) {//using the menu to blink the led
    nav.doNav(navCmd(idxCmd,1));
    nav.doNav(navCmd(enterCmd));
    delay(300);
    nav.doNav(navCmd(idxCmd,3));
    nav.doNav(navCmd(enterCmd));
    delay(300);
  }

}

void loop() {

  nav.poll();

}
