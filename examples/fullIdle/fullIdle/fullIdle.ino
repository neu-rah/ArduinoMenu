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

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
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
  inline void reset() {next=millis()+step;}
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
    default:break;
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
    //do your own idle state stuff here
    //....
    //we can now use encoder, buttons, serial, display or whatever input/output we have
    // because menu is 100% dormant

    //call `nav.idleOff()` to terminate the idle when some condition is met
    //... here using a timeout because we have no buttons
    Serial.write(Serial.available()?(char)Serial.read():'.');
    delay(200);
    if(idleTimeout) {
      Serial.println();
      Serial.println("Terminating full idle by timeout!");
      Serial.println();
      nav.idleOff();
    }
  } else if(menuFPS) nav.poll();
}
