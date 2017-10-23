#include "menu.h"
using namespace Menu;

const char* Menu::numericChars="0123456789.";

result Menu::doNothing() {return proceed;}
result Menu::doExit() {return quit;}
action Menu::noAction(doNothing);

//this is for idle (menu suspended)
result Menu::inaction(menuOut& o,idleEvent) {
  trace(Serial<<"Menu::inaction"<<endl);
  return proceed;
}

result Menu::maxDepthError(menuOut& o,idleEvent e) {
  trace(Serial<<"maxDepthError"<<endl);
  o.print(F("Error: maxDepth reached!\n\rincrease maxDepth on your scketch."));
  return proceed;
}
