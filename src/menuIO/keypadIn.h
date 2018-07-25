/* -*- C++ -*- */
/**************
Jan. 2018 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

keypad driver
using
  http://playground.arduino.cc/Code/Keypad
  https://github.com/Nullkraft/Keypad

***/

#ifndef RSITE_KEYPAD_DEF_H
  #define RSITE_KEYPAD_DEF_H
  #include "../menuDefs.h"

  #include <Keypad.h>
  namespace Menu {

    class keypadIn:public menuIn {
    public:
      char key=0;
      Keypad& in;
      keypadIn(Keypad& in):in(in) {}
      int available(void) {return peek()!=-1;}
      int peek(void) {return key?key:(key=in.getKey())?key:-1;}
      int read() {
        if (key) {
          char k=key;
          key=0;
          return k;
        }
        return available()?read():-1;
      }
      void flush() {}
      size_t write(uint8_t v) {return 0;}
    };
  };
#endif
