#ifndef RSITE_ARDUINO_MENU_ANSISERIALOUT
  #define RSITE_ARDUINO_MENU_ANSISERIALOUT
  #include "serialOut.h"

  namespace Menu {
    class ansiSerialOut:public serialOut {
      public:
        inline ansiSerialOut(Print& o):serialOut(o) {}
        virtual void printMenu(navNode &nav);
    };

  }//namespace Menu
#endif
