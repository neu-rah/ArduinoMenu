/* List of Supported Fonts

  Arial14,
  Arial_bold_14,
  Callibri11,
  Callibri11_bold,
  Callibri11_italic,
  Callibri15,
  Corsiva_12,
  fixed_bold10x15,
  font5x7,
  font8x8,
  Iain5x7,
  lcd5x7,
  Stang5x7,
  System5x7,
  TimesNewRoman16,
  TimesNewRoman16_bold,
  TimesNewRoman16_italic,
  utf8font10x16,
  Verdana12,
  Verdana12_bold,
  Verdana12_italic,
  X11fixed7x14,
  X11fixed7x14B,
  ZevvPeep8x16

*/

//Library Dependency : SSD1306Ascii by Bill Greiman
//Link : https://github.com/greiman/SSD1306Ascii
// Modified sketch by Tamojit Saha(https://github.com/TamojitSaha)


#include <Arduino.h>

constexpr int OLED_SDA=4;
constexpr int OLED_SDC=5;

#define I2C_ADDRESS 0x3C
// #include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/serialIO.h>
//enable this include if using esp8266
// #include <menuIO/esp8266Out.h>
using namespace Menu;

//Define your font here. Default font: lcd5x7
#define menuFont X11fixed7x14
#define fontW 7
#define fontH 15

// #define menuFont System5x7
// #define menuFont lcd5x7
// #define fontW 5
// #define fontH 8

SSD1306AsciiWire oled;

result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e, navNode& nav, prompt& item) {
  Serial.print("event: ");
  Serial.println(e);
  return proceed;
}

int test = 55;

result action1(eventMask e) {
  Serial.print(e);
  Serial.println(" action1 executed, proceed menu"); Serial.flush();
  trace(if (e == enterEvent) oled.clear(0, oled.displayWidth(), 2, 3));
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item) {
  Serial.print(e);
  Serial.println(" action2 executed, quiting menu");
  return quit;
}

int ledCtrl = LOW;

result myLedOn() {
  ledCtrl = HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl = LOW;
  return proceed;
}

TOGGLE(ledCtrl, setLed, "Led: ", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle
       , VALUE("On", HIGH, doNothing, noEvent)
       , VALUE("Off", LOW, doNothing, noEvent)
      );

int selTest = 0;
SELECT(selTest, selMenu, "Select", doNothing, noEvent, noStyle
       , VALUE("Zero", 0, doNothing, noEvent)
       , VALUE("One", 1, doNothing, noEvent)
       , VALUE("Two", 2, doNothing, noEvent)
      );

int chooseTest = -1;
CHOOSE(chooseTest, chooseMenu, "Choose", doNothing, noEvent, noStyle
       , VALUE("First", 1, doNothing, noEvent)
       , VALUE("Second", 2, doNothing, noEvent)
       , VALUE("Third", 3, doNothing, noEvent)
       , VALUE("Last", -1, doNothing, noEvent)
      );

//customizing a prompt look!
//by extending the prompt class
class altPrompt: public prompt {
  public:
    altPrompt(constMEM promptShadow& p): prompt(p) {}
    Used printTo(navRoot &root, bool sel, menuOut& out, idx_t idx, idx_t len, idx_t) override {
      return out.printRaw(F( "special prompt!"), len);
    }
};

MENU(subMenu, "Sub-Menu", showEvent, anyEvent, noStyle
     , OP("Sub1", showEvent, anyEvent)
     , OP("Sub2", showEvent, anyEvent)
     , OP("Sub3", showEvent, anyEvent)
     , altOP(altPrompt, "", showEvent, anyEvent)
     , EXIT("<Back")
    );

MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
     , OP("Op1", action1, anyEvent)
     , OP("Op2", action2, enterEvent)
     , FIELD(test, "Test", "%", 0, 100, 10, 1, doNothing, noEvent, wrapStyle)
     , SUBMENU(subMenu)
     , SUBMENU(setLed)
     , OP("LED On", myLedOn, enterEvent)
     , OP("LED Off", myLedOff, enterEvent)
     , SUBMENU(selMenu)
     , SUBMENU(chooseMenu)
     , OP("Alert test", doAlert, enterEvent)
     , EXIT("<Back")
    );

#define MAX_DEPTH 2

//define output device
idx_t serialTops[MAX_DEPTH] = {0};
serialOut outSerial(Serial, serialTops);

//describing a menu output device without macros
//define at least one panel for menu output
const panel panels[] MEMMODE = {{0, 0, 128 / fontW, 64 / fontH}};
navNode* nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, 1); //a list of panels and nodes
idx_t tops[MAX_DEPTH] = {0, 0}; //store cursor positions for each level
SSD1306AsciiOut outOLED(&oled, tops, pList, 8, 1+((fontH-1)>>3) ); //oled output device menu driver
menuOut* constMEM outputs[] MEMMODE = {&outOLED, &outSerial}; //list of output devices
outputsList out(outputs, sizeof(outputs) / sizeof(menuOut*)); //outputs list

//macro to create navigation control root object (nav) using mainMenu
serialIn serial(Serial);
NAVROOT(nav, mainMenu, MAX_DEPTH, serial, out);

result alert(menuOut& o, idleEvent e) {
  if (e == idling) {
    o.setCursor(0, 0);
    o.print("alert test");
    o.setCursor(0, 1);
    o.print("press [select]");
    o.setCursor(0, 2);
    o.print("to continue...");
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}

//when menu is suspended
result idle(menuOut &o, idleEvent e) {
  o.clear();
  if (&o==&outOLED) {
    if (e==idling) {
      o.println("OLED");
      o.println("Suspended menu");
    }
  } else
    switch (e) {
      case idleStart: o.println("suspending menu!"); break;
      case idling: o.println("suspended..."); break;
      case idleEnd: o.println("resuming menu."); break;
    }
  return proceed;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("menu 4.x test"); Serial.flush();
  pinMode(A0, INPUT);
  Wire.begin();
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(menuFont);

  oled.clear();
  oled.setCursor(0, 0);
  oled.print("menu 4.x test");
  oled.setCursor(0, 2);
  oled.print("SSD1306Ascii");
  delay(2000);
  oled.clear();
  nav.idleTask = idle; //point a function to be used when menu is suspended
}

void loop() {
  //if (!digitalRead(A0)) action1(enterEvent);
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
