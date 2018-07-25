/* -*- C++ -*- */
/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

analogAxisIn - analog joystick axis
*/

#ifndef RSITE_ARDUINO_MENU_ANALOG_JOYSTICK_INPUT
  #define RSITE_ARDUINO_MENU_ANALOG_JOYSTICK_INPUT
  #include "../menuDefs.h"

  namespace Menu {

    template<
      uint8_t pin,//axis pin number
      uint8_t sensivity=5,
      bool inv=false,//invert axis direction
      int center=512,//calibration
      int inertia=6,//prevent rest drift
      int bufferSz=1,//how many clicks can accumulate
      navCmds posCmd=upCmd,//what is sent by positive axis click
      navCmds negCmd=downCmd//what is sent by negative axis click
    > class analogAxis:public menuIn {
      public:
        bool field_mode=false;
        virtual void setFieldMode(bool mode) {field_mode=mode;}
        virtual bool fieldMode() const {return field_mode;}
        int cnt=0;
        long last=0;
        void getStep() {
          uint16_t p=analogRead(pin);
          int a=p-center;
          a=a>0?a:-a;
          if (!(a>>inertia)) return;
          float s=1000000.0/a/(sensivity+1);
          long m=millis();
          long dt=m-last;
          if (abs(cnt)<=bufferSz&&dt>s) {
            cnt+=p>center?1:-1;
            last=m;
          }
          // MENU_DEBUG_OUT<<endl<<"dt:"<<dt<<" >s:"<<s;
        }
        inline navCmds pos() const {return inv^field_mode?negCmd:posCmd;}
        inline navCmds neg() const {return inv^field_mode?posCmd:negCmd;}
        int available(void) {getStep();return cnt!=0;}
        int peek(void) override {
          getStep();
          if (cnt<0) return options->navCodes[neg()].ch;
          if (cnt>0) return options->navCodes[pos()].ch;
          return -1;
        }
        int read() override {
          getStep();
          if (cnt<0) {
            cnt++;
            return options->navCodes[neg()].ch;
          }
          if (cnt>0) {
            cnt--;
            return options->navCodes[pos()].ch;
          }
          return -1;
        }
        void flush() {cnt=0;last=millis();}
        size_t write(uint8_t v) {cnt=v;return 1;}
    };
  }//namespace Menu

#endif
