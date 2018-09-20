/* -*- C++ -*- */
/**************
Jan. 2018 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

I2C keypad driver
using
   https://github.com/whatnick/i2ckeypad

   Note: you may be need to adjust keypad-to-pcf8574 pin mappings and keymap in i2ckeypad.cpp for your specific keypad.

***/

#ifndef RSITE_I2CKEYPAD_DEF_H
  #define RSITE_I2CKEYPAD_DEF_H
  #include "../menuDefs.h"

  #include <i2ckeypad.h>
  namespace Menu {

    class I2CkeypadIn:public menuIn {
    public:
      char key=0;
      i2ckeypad * in;
      I2CkeypadIn(i2ckeypad * in):in(in) {}
      int available(void) {
		  return peek()!=0;
	  }
      int peek(void) { 
		  return key != 0 ? key : (key=in->get_key()) ?key : 0;
	  }
      int read() {
        if (key != 0) {
          char k=key;
          key=0;
          return k;
        }
        return available()?read():0;
      }
      void flush() {}
      size_t write(uint8_t v) {return 0;}
    };
  };
#endif
