/* -*- C++ -*- */
/********************
May 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
www.r-site.net
unlike native objects, pluggins can be device specific, so some pluggins might
not work on all devices.
all sorts of plugins may be available in the future and if you customize
a component and think it of interest of others please do pull request.
contrubutions and contrubutors are welcome.

decimalslField - To select the number of decimal places that are printed in float and double variables types. Feb 2019 ferchinas
                 Tested on ESP32, Oled i2c display, u8g2 driver. It could work on other devices.
***/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define MAX_DEPTH 1

unsigned char cantDecimals = 1;
float         floatVar     = 0.123456789123456789;
double        doubletVar   = 0.123456789123456789;

void updateEvent_cb(eventMask e);

//-----Custom floatField----------------
#define DECIMALSFLIED_DEFAULT 1

template<typename T>
class decimalslField :public menuField<T> {                //https://github.com/neu-rah/ArduinoMenu/blob/master/examples/customField/customField/customField.ino
private:
  idx_t decimals;
public:
  decimalslField(constMEM menuFieldShadow<T>& shadow) :menuField<T>(shadow) { decimals = DECIMALSFLIED_DEFAULT; }
  decimalslField(
    T &value,
    constText* text,
    constText*units,
    T low,
    T high,
    T step,
    T tune,
    action a = doNothing,
    eventMask e = noEvent,
    styles s = noStyle
  ) :decimalslField(*new menuFieldShadow<T>(value, text, units, low, high, step, tune, a, e, s)) {}

  Used printTo(navRoot &root, bool sel, menuOut& out, idx_t idx, idx_t len, idx_t panelNr = 0) override {// https://github.com/neu-rah/ArduinoMenu/issues/94#issuecomment-290936646
    //menuFieldShadow<T>& s=*(menuFieldShadow<T>*)shadow;
    menuField<T>::reflex = menuField<T>::target();
    idx_t l = prompt::printTo(root, sel, out, idx, len);
    // bool ed = this == root.navFocus;
    //bool sel=nav.sel==i;
    if (l < len) {
      out.print((root.navFocus == this&&sel) ? (menuField<T>::tunning ? '>' : ':') : ' ');
      l++;
      if (l < len) {
        l += out.print(menuField<T>::reflex, decimals);//NOTE: this can exceed the limits!
        if (l < len) {
          l += print_P(out, fieldBase::units(), len);
        }
      }
    }
    return l;
  }

  void setDecimals(idx_t d) { decimals = d; }
  idx_t getDecimals(void) { return(decimals); }
};
//-----Custom floatField----------------END


decimalslField <typeof(floatVar)>& floatFlied_Object = *new decimalslField<typeof(floatVar)>(floatVar, "f", "", -100.0, 100.0, 0.1, 1);
decimalslField <typeof(doubletVar)>& doubleFlied_Object = *new decimalslField<typeof(doubletVar)>(doubletVar, "d", "", -100.0, 100.0, 0.1, 1);
decimalslField <typeof(cantDecimals)>& DecimalsFlied_Object = *new decimalslField<typeof(cantDecimals)>(cantDecimals, "Dec", "", 0, 18, 1, 1, (Menu::callback)updateEvent_cb, updateEvent);

void updateEvent_cb(eventMask e){
  floatFlied_Object.setDecimals(cantDecimals);
  doubleFlied_Object.setDecimals(cantDecimals);
  Serial.println("");Serial.print("now using '");Serial.print(cantDecimals);Serial.println("' decimals.");
 }

//---Main menu  ---------------------------------
prompt* mainData[] = {
  &floatFlied_Object,
  &doubleFlied_Object,
  &DecimalsFlied_Object,
  new Exit("<Back")
};

menuNode& mainMenu = *new menuNode("Main menu", sizeof(mainData) / sizeof(prompt*), mainData);

serialIn serial(Serial);
MENU_INPUTS(in,&serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 4.x");
  Serial.println("Use keys + - * /");
  Serial.println("to control the menu navigation");
  nav.useUpdateEvent = true;
}

void loop() {
  nav.poll();
}
