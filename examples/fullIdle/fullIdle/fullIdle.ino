/********************
Arduino generic menu system
using menu idle to do complex tasks

Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

output: Serial
input: Serial
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

void showPath(navRoot& root) {
  Serial.print("nav level:");
  Serial.print(root.level);
  Serial.print(" path:[");
  for(int n=0;n<=root.level;n++) {
    Serial.print(n?",":"");
    Serial.print(root.path[n].sel);
  }
  Serial.println("]");
}

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial.println();
  Serial.println("========");
  Serial.print("Event for target: 0x");
  Serial.println((long)nav.target,HEX);
  showPath(*nav.root);
  Serial.print(e);
  switch(e) {
    case noEvent://just ignore all stuff
      Serial.println(" noEvent");break;
    case activateEvent://this item is about to be active (system event)
      Serial.println(" activateEvent");break;
    case enterEvent://entering navigation level (this menu is now active)
      Serial.println(" enterEvent");break;
    case exitEvent://leaving navigation level
      Serial.println(" exitEvent");break;
    case returnEvent://TODO:entering previous level (return)
      Serial.println(" returnEvent");break;
    case focusEvent://element just gained focus
      Serial.println(" focusEvent");break;
    case blurEvent://element about to lose focus
      Serial.println(" blurEvent");break;
    case selFocusEvent://TODO:child just gained focus
      Serial.println(" selFocusEvent");break;
    case selBlurEvent://TODO:child about to lose focus
      Serial.println(" selBlurEvent");break;
    case updateEvent://Field value has been updated
      Serial.println(" updateEvent");break;
    case anyEvent:
      Serial.println(" anyEvent");break;
  }
  return proceed;
}

MENU(mainMenu,"Main menu",showEvent,anyEvent,wrapStyle
  ,OP("Op1",showEvent,anyEvent)
  ,OP("Op2",showEvent,anyEvent)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

//////////////////////////////////////////////////////
// some util functions
//time ticks as boolean
template<int step>
struct Tick {
  inline operator bool() {
    return millis()>=next?next+=step,true:false;
  }
  inline reset() {next=millis()+step;}
protected:
  unsigned long next=0;
};

//FPS as boolean
// this is an FPS limiter
//return `true` `fps` times per second (max)
//note: this can skip a frame at millis wrap (every 50days ;))
template<int fps>
struct FPS:public Tick<(1000/fps)> {};
//-----------------------------------------------------

FPS<25> menuFPS;//limit menu pool to 25 frames per second
Tick<10000> idleTimeout;

result idle(menuOut &o, idleEvent e) {
  // o.clear();
  switch(e) {
    case idleStart:
      o.println("suspending menu!");
      idleTimeout.reset();
      break;
    case idleEnd:
      o.println("resuming menu.");
      // nav.reset(); clear nav state if desired
      break;
  }
  return proceed;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x test");Serial.flush();
  nav.timeOut=5;//made very short for demo purposes
  nav.idleTask=idle;//point a function to be used when menu is suspended
  // nav.idleOn();//this menu will start on idle state, press select to enter menu
}

void loop() {
  if(nav.sleepTask) {
    //do idle state stuff here
    //....
    //we can now use encoder, buttons, serial, dosplay or whatever input/output we have
    // because menu is 100% dormant

    //call `nav.idleOff()` to terminate the idle when some condition is met
    //.., here using a timeout because we have no buttons
    Serial.write(Serial.available()?(char)Serial.read():'.');
    delay(200);
    if(idleTimeout) {
      Serial.println();
      Serial.println("Terminating full idle!");
      Serial.println();
      nav.idleOff();
    }
  } else if(menuFPS) nav.poll();
}
