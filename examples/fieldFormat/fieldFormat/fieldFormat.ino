#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
// #include <Streaming.h>

using namespace Menu;

int test=55;

//custom field print
//implementing a customized menu component
//this numeric field prints formatted number with leading zeros
template<typename T>
class leadsField:public menuField<T> {
public:
  using menuField<T>::menuField;
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override {
    menuField<T>::reflex=menuField<T>::target();
    prompt::printTo(root,sel,out,idx,len);
    bool ed=this==root.navFocus;
    out.print((root.navFocus==this&&sel)?(menuField<T>::tunning?'>':':'):' ');
    out.setColor(valColor,sel,menuField<T>::enabled,ed);
    char buffer[]="      ";
    sprintf(buffer, "%03d", menuField<T>::reflex);
    out.print(buffer);
    out.setColor(unitColor,sel,menuField<T>::enabled,ed);
    print_P(out,menuField<T>::units(),len);
    return len;
  }
};

//a menu using a customized components
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,altFIELD(leadsField,test,"Custom edit","%",0,100,10,1,doNothing,enterEvent,wrapStyle)
  ,FIELD(test,"Original edit","%",0,100,10,1,doNothing,noEvent,wrapStyle)
);

#define MAX_DEPTH 1

serialIn serial(Serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

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
