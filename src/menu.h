#define DEBUG

#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

#include "menu.hpp"
//#include "menuBase.hpp"
//#include "macros.hpp"
//#include "baseMacros.hpp"
#include "dev/adafruitGfxOut.hpp"
#include "dev/ansiSerialOut.hpp"
#include "dev/chainStream.hpp"
#include "dev/encoderIn.hpp"
//#include "dev/htmlOut.hpp"
#include "dev/keyIn.hpp"
#include "dev/lcdOut.hpp"
#include "dev/serialOut.hpp"
#include "dev/U8GLibOut.hpp"
