/* -*- C++ -*- */

/**************
todo: write description
available keys from 0 to 7 correspond with pcf8574 input pins from p0 to p7

example:

#include <menuIO/PCF8574KeyIn.h>

#define K_UP     0
#define K_RIGHT  1
#define K_LEFT   2
#define K_DOWN   3

#define K_ESC    4
#define K_ENTER  5

keyMap myBtn_map[]={
                      {K_UP,options->getCmdChar(upCmd)},
                      {K_RIGHT,options->getCmdChar(rightCmd)},
                      {K_LEFT,options->getCmdChar(leftCmd)},
                      {K_DOWN,options->getCmdChar(downCmd)},
                      {K_ESC,options->getCmdChar(escCmd)},
                      {K_ENTER,options->getCmdChar(enterCmd)}
                    };
PCF8574KeyIn<6> myButton(myBtn_map);

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
    private:
      bool modeEdit;  
    public:
      keyMap* keys;
      int lastkey;
      unsigned long pressMills=0;
      PCF8574KeyIn<N, _dev, _sda, _scl>(keyMap k[]):keys(k),lastkey(-1) {}
      void setFieldMode(bool mode) override {
        modeEdit = mode;
      }
      void begin() {
        Wire.begin(_sda, _scl);
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
        Wire.requestFrom(_dev, 1);
        uint8_t val = Wire.read();
        for(int n=0;n<N;n++) {
          int8_t pin = keys[n].pin;
          int8_t code = keys[n].code;
          if (((val)&1<<pin)==0) {
            //Serial<<"key "<<pin<<" pressed"<<endl;
            if (modeEdit == false && (code == options->getCmdChar(upCmd) || navCmds(code) == options->getCmdChar(downCmd))) {
              //if we navigate on menu then return code up/down (left/right) reversed as down/up (right/left)
              return (code==options->getCmdChar(upCmd)?options->getCmdChar(downCmd):options->getCmdChar(upCmd));
            }
            else
            {
              //else if we inside edit field then return all key codes as is
              return code;
            }
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
