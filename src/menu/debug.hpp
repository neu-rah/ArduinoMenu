#include "api.h"

#ifdef MENU_DEBUG
  #ifdef ARDUINO
    DebugOut<decltype(Serial),Serial> debugOut;
  #else
    DebugOut<decltype(cout),cout> debugOut;
  #endif
#endif
