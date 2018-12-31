#include "menuDefs.h"
using namespace Menu;

const char* Menu::numericChars="0123456789.";

result Menu::doNothing() {return proceed;}
result Menu::doExit() {return quit;}
action Menu::noAction(doNothing);

//this is for idle (menu suspended)
result Menu::inaction(menuOut& o,idleEvent) {
  trace(MENU_DEBUG_OUT<<"Menu::inaction"<<endl);
  return proceed;
}

result Menu::maxDepthError(menuOut& o,idleEvent e) {
  trace(MENU_DEBUG_OUT<<"maxDepthError"<<endl);
  o.print("Error: maxDepth reached!\n\rincrease maxDepth on your scketch.");
  return proceed;
}

#if defined(MENU_DEBUG) || defined(MENU_ASYNC)
  String& operator<<(String&s,prompt &p) {return s+=p.getText();}
  Stream& operator<<(Stream&o,prompt& p) {print_P(o,p.getText());return o;}
  Print& operator<<(Print&o,prompt& p) {print_P(o,p.getText());return o;}
#endif
