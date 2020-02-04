#include "api.h"

#ifdef MENU_DEBUG
  #ifdef ARDUINO
    DebugOut<decltype(Serial),Serial>::Part<> debugOut;
  #else
    DebugOut<decltype(cout),cout>::Part<> debugOut;
  #endif
#endif
