/* -*- C++ -*- */
/********************
Feb.2018 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
www.r-site.net

string input driver
***/

#ifndef RSITE_ARDUINO_MENU_HW_STRINGIN
  #define RSITE_ARDUINO_MENU_HW_STRINGIN
  #include "../menuDefs.h"

  namespace Menu {
    //using a circular buffer
    //attention, if using sz=0 (2^0=1) we just override previous character
    template<uint8_t sz=0>//this size is 2^sz
    class stringIn:public menuIn {
      public:
        static_assert(sz<7,"ArduinoMenu: max buffer size is 2^7");
        enum Mask:uint8_t {
          nBytes=1<<sz,
          mask=nBytes-1
        };
        char data[nBytes]={0};
        uint8_t head=0;
        uint8_t at=0;
        size_t write(uint8_t o) override {
          if (sz&&(at==mask&(head-1))) return 0;//buffer full!
          data[at]=o;
          at++;
          at&=mask;
          return 1;
        }
        int available() override {return sz?at-head:data[head]!=0;}
        int peek() override {return available()?data[head]:0;}
        int read() override {
          if (available()) {
            if (sz&&(head==at)) return -1;
            int r=data[head];
            if(!sz) data[head]=0;
            head++;
            head&=mask;
            return r;
          }
          return -1;
        }
        void flush() override {at=head=0;}//just throw away all data!
    };
  }

#endif
