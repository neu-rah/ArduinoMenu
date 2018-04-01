/* -*- C++ -*- */
/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

quadrature encoder driver (PCINT)
quadrature encoder stream (fake, not using buffers)

*/

#ifndef RSITE_ARDUINO_MENU_ENCODER
  #define RSITE_ARDUINO_MENU_ENCODER

  #include <pcint.h> //https://github.com/neu-rah/PCINT
  #include "../menu.h"

  namespace Menu {

    template<uint8_t pinA,uint8_t pinB>
    class encoderDrv {
    public:
      volatile int pos=0;
      //int pinA,pinB;
      //encoderIn<pinA,pinB>(int a,int b):pinA(a),pinB(b) {}
      void begin() {
        pinMode(pinA, INPUT_PULLUP);
        pinMode(pinB, INPUT_PULLUP);
        //attach pin change handlers
        PCattachInterrupt<pinA>(mixHandler((void(*)(void*))encoderInUpdateA,this), CHANGE);
        PCattachInterrupt<pinB>(mixHandler((void(*)(void*))encoderInUpdateB,this), CHANGE);
      }
      //PCint handlers
      static void encoderInUpdateA(class encoderDrv<pinA,pinB> *e);
      static void encoderInUpdateB(class encoderDrv<pinA,pinB> *e);
    };

    //PCint handlers
    template<uint8_t pinA,uint8_t pinB>
    void encoderDrv<pinA,pinB>::encoderInUpdateA(class encoderDrv<pinA,pinB> *e) {
      if (digitalRead(pinA)^digitalRead(pinB)) e->pos--;
      else e->pos++;
    }

    template<uint8_t pinA,uint8_t pinB>
    void encoderDrv<pinA,pinB>::encoderInUpdateB(class encoderDrv<pinA,pinB> *e) {
      if (digitalRead(pinA)^digitalRead(pinB)) e->pos++;
      else e->pos--;
    }

    template<uint8_t pinA,uint8_t pinB>
    class encoderIn:public menuIn {
    public:
      encoderDrv<pinA,pinB> &enc;//associated hardware encoderIn
      int sensivity;
      int oldPos=0;
      encoderIn(encoderDrv<pinA,pinB> &enc,int sensivity):enc(enc), sensivity(sensivity) {}
      inline void setSensivity(int s) {sensivity=s;}
      int available(void) {return abs(enc.pos-oldPos)/sensivity;}
      navCmds peek(void) override {
        int d=enc.pos-oldPos;
        if (d<=-sensivity)return downCmd;
        if (d>=sensivity) return upCmd;
        return noCmd;
      }
      navCmds getCmd() override {
        int d=enc.pos-oldPos;
        if (d<=-sensivity) {
          oldPos-=sensivity;
          return downCmd;
        }
        if (d>=sensivity) {
          oldPos+=sensivity;
          return upCmd;
        }
        return noCmd;
      }

    };

  }//namespace Menu
#endif
