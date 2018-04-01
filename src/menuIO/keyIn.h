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

    struct keyMap {
      int8_t pin;
      navCmds cmd;
    };

    // #include "keyMapDef.h"

    //if you hold/repeat a key for this ammount of time we will consider it an escape
    #ifndef ESCAPE_TIME
    #define ESCAPE_TIME 1500
    #endif
    //emulate a stream keyboard, this is not using interrupts as a good driver should do
    // AND is not using a buffer either!
    template <int N>
    class keyIn:public menuIn {
    public:
      keyMap* keys;
      navCmd lastkey;
      unsigned long pressMills=0;
      keyIn<N>(keyMap k[]):keys(k),lastkey(noCmd) {}
      void begin() {
        for(int n=0;n<N;n++)
          if (keys[n].pin<0) pinMode(-keys[n].pin,INPUT_PULLUP);
          else pinMode(keys[n].pin,INPUT);
      }
      int available(void) {
        //Serial<<"available"<<endl;
        navCmd ch=peek();
        if (lastkey==noCmd) {
          lastkey=ch;
          pressMills=millis();
        } else if (ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME) return 1;
        if (ch==lastkey) return 0;
        return 1;
        /*int cnt=0;
        for(int n=0;n<N;n++) {
          int8_t pin=keys[n].pin;
          if (digitalRead(pin<0?-pin:pin)!=(pin<0) ) cnt++;
        }
        return cnt;*/
      }
      navCmd peek(void) {
        //Serial<<"peek"<<endl;
        for(int n=0;n<N;n++) {
          int8_t pin=keys[n].pin;
          if (digitalRead(pin<0?-pin:pin)!=(pin<0) ) return keys[n].cmd;
        }
        return -1;
      }
      navCmd getCmd() {
        //Serial<<"read"<<endl;
        navCmd ch=peek();
        if (ch==lastkey) return -1;
        navCmd tmp=lastkey;
        bool longPress=ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME;
        //Serial<<"read lastkey="<<lastkey<<" ch="<<ch<<endl;
        //Serial<<"down time:"<<millis()-pressMills<<endl;
        pressMills=millis();
        lastkey=ch;
        return longPress?escCmd:tmp;//long press will result in escape
      }
      // void flush() {}
      // size_t write(uint8_t v) {return 0;}
    };

  }//namespace Menu

#endif
