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
  #include "../menu.h"

  #include <Keypad.h>
  namespace Menu {

    class keypadIn:public menuIn {
    public:
      char key=0;
      Keypad& in;
      keypadIn(Keypad& in):in(in) {}
      int available(void) {return peek()!=0;}
      int peek(void) {
        int ret;

        if (key) {
          return key;
        } else {
          key = (int)in.getKey();
          if (key) {
            return key;
          } else {
            return -1;
          }
        }
      }
      int read() {
        if (key) {
          char k=key;
          key=0;
          return k;
        }
        return peek();
      }
      void flush() {}
      size_t write(uint8_t v) {return 0;}
    };
  };
#endif
