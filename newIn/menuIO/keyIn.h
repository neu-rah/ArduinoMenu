/* -*- C++ -*- */
/**************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

quick and dirty keyboard driver
metaprog keyboard driver where N is the number of keys
all keys are expected to be a pin (buttons)
we can have reverse logic (pull-ups) by entering negative pin numbers
ex: -A0 means: pin A0 normally high, low when button pushed (reverse logic)

***/

#ifndef RSITE_KEYIN_DEF_H
  #define RSITE_KEYIN_DEF_H
  #include "../menu.h"

  namespace Menu {

    //map of keyu code to navCmds
    struct keyMap {
      int8_t pin;
      navCmds code;
    };
    //if you hold/repeat a key for this ammount of time we will consider it an escape
    #ifndef ESCAPE_TIME
    #define ESCAPE_TIME ET
    #endif
    //emulate a stream keyboard, this is not using interrupts as a good driver should do
    // AND is not using a buffer either!
    template <int N,int ET=1500>
    class keyIn:public menuIn {
    public:
      keyMap* keys;
      navCmd lastCmd;
      unsigned long pressMills=0;
      keyIn<N,ET>(keyMap k[]):keys(k),lastCmd(noCmd) {}
      void begin() {
        for(int n=0;n<N;n++)
          if (keys[n].pin<0) pinMode(-keys[n].pin,INPUT_PULLUP);
          else pinMode(keys[n].pin,INPUT);
      }
      int available() override {
        //Serial<<"available"<<endl;
        navCmd cmd=peek();
        if (lastCmd==noCmd) {
          lastCmd=cmd;
          pressMills=millis();
        } else if (ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME) return 1;
        if (cmd==lastCmd) return 0;
        return 1;
      }
      navCmds peek() override {
        for(int n=0;n<N;n++) {
          // Serial<<"keyIn::peek"<<endl;
          int8_t pin=keys[n].pin;
          if (digitalRead(pin<0?-pin:pin)!=(pin<0) ) return keys[n].code;
        }
        return noCmd;
      }
      navCmds getCmd() override {
        int cmd=peek();

        if (cmd==lastCmd) {
          bool longPress=ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME;
          pressMills=millis();
          return longPress?escCmd:noCmd;//do not repeat
        }
        Serial<<"keyIn::getCmd"<<endl;
        lastCmd=cmd;
        pressMills=millis();
        return lastCmd;
      }
    };

  }//namespace Menu

#endif
