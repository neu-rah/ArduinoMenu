#pragma once
#ifdef ESP8266
  #include "../menuDefs.h"
  //MENU_FMT_WRAPS
  #include "esp8266Out.h"

  menuOut& operator<<(menuOut&o,classes c) {
    switch(c) {
      case noClass:return o<<"noClass";break;
      case promptClass:return o<<"prompt";break;
      case fieldClass:return o<<"field";break;
      case toggleClass:return o<<"toggle";break;
      case selectClass:return o<<"select";break;
      case chooseClass:return o<<"choose";break;
      case valueClass:return o<<"value";break;
      case menuClass:return o<<"menu";break;
      default: return o;
    }
  }
#endif
