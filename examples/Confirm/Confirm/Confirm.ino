/*

Custom sub-menu prompt used as exit confirmation
the Exit option on this example presents only the "Exit" text
but the submenu really as an "Exit?" text for confirmation
choosing "Yes" will suspend the menu and possibly do other stuff
while "Cancel" will just return to previous menu.

*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

//customizing a menu prompt look
class confirmExit:public menu {
public:
  confirmExit(constMEM menuNodeShadow& shadow):menu(shadow) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t p) override {
    return idx<0?//idx will be -1 when printing a menu title or a valid index when printing as option
      menu::printTo(root,sel,out,idx,len,p)://when printing title
      out.printRaw((constText*)F("Exit"),len);//when printing as regular option
  }
};

// this function is defined below because we need to refer
// to the navigation system (suspending the menu)
result systemExit();

//using the customized menu class
//note that first parameter is the class name
altMENU(confirmExit,subMenu,"Exit?",doNothing,noEvent,wrapStyle,(Menu::_menuData|Menu::_canNav)
  ,OP("Yes",systemExit,enterEvent)
  ,EXIT("Cancel")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,SUBMENU(subMenu)
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

bool running=true;//lock menu if false

result systemExit() {
  Serial.println();
  Serial.println("Terminating...");
  //do some termiination stuff here
  running=false;//prevents the menu from running again!
  Serial.println("done.");
  nav.idleOn();//suspend the menu system
  return quit;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x custom sub-menu prompt example");Serial.flush();
}

void loop() {
  if (running) nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
