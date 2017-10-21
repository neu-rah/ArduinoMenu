/* -*- C++ -*- */

/**************
todo: write description
***/

#ifndef __PCF8574KeyIn_h__
  #define __PCF8574KeyIn_h__

  #include "../menu.h"
  #include <Wire.h>

  namespace Menu {

    #include "keyMapDef.h"

    #ifndef BOUNCE_TICK
    #define BOUNCE_TICK 30
    #endif

    //if you hold/repeat a key for this ammount of time we will consider it an escape
    #ifndef ESCAPE_TIME
    #define ESCAPE_TIME 1500
    #endif
    //emulate a stream keyboard, this is not using interrupts as a good driver should do
    // AND is not using a buffer either!
    template <int N, int _dev=0x20, int _sda=SDA, int _scl=SCL>                 //default pcf8574 address=0x20
    class PCF8574KeyIn:public menuIn {
    public:
      keyMap* keys;
      int lastkey;
      TwoWire iicWire;
      unsigned long pressMills=0;
      PCF8574KeyIn<N, _dev, _sda, _scl>(keyMap k[]):keys(k),lastkey(-1) {}
      void begin() {
        iicWire.begin(_sda, _scl);
      }
      int available(void) {
        int ch=peek();
        if (lastkey==-1) {
          lastkey=ch;
          pressMills=millis();
        }
        else if (ch == -1 && millis()-pressMills < BOUNCE_TICK)
        {
          lastkey = -1;  //released = it's bounce. reset lastkey
          return 0;
        }
        else if (ch != -1 && millis()-pressMills > BOUNCE_TICK) return 1;
        else if (ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME) return 1;

        if (ch==lastkey) return 0;

        return 1;
      }
      int peek(void) {
        //Serial<<"peek"<<endl;

        iicWire.requestFrom(_dev, 1);
        uint8_t val = iicWire.read();

        for(int n=0;n<N;n++) {
          int8_t pin=keys[n].pin;
          if (((val)&1<<pin)==0) {
            //Serial<<"key "<<pin<<" pressed"<<endl;
            return keys[n].code;
          }
        }
        return -1;
      }
      int read() {
        //Serial<<"read"<<endl;
        int ch=peek();
        if (ch==lastkey) return -1;
        int tmp=lastkey;
        bool longPress=ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME;
        //Serial<<"read lastkey="<<lastkey<<" ch="<<ch<<endl;
        //Serial<<"down time:"<<millis()-pressMills<<endl;
        pressMills=millis();
        lastkey=ch;
        return longPress?options->getCmdChar(escCmd):tmp;//long press will result in escape
      }
      void flush() {}
      size_t write(uint8_t v) {return 0;}
    };

  }//namespace Menu

#endif
