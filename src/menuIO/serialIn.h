/* -*- C++ -*- */
/********************
Sept 2014 ~ Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
www.r-site.net

HardwareSerial input driver
***/

#ifndef RSITE_ARDUINO_MENU_HW_SERIALIN
  #define RSITE_ARDUINO_MENU_HW_SERIALIN
  #include "../menu.h"

  #define SERIAL_CLASS Stream

  namespace Menu {
    class serialIn:public streamIn {
      public:
        Stream& in;
        serialIn(Stream& serial):in(serial) {}
        int available() override {
          trace(Serial<<"serialIn::available"<<endl);
          return in.available()?*this:false;
        }
        navCmd peek() override {
          trace(Serial<<"serialIn::peek"<<endl);
          return navKeys(in.peek());
        }
        navCmd getCmd() override {
          trace(Serial<<"serialIn::getCmd"<<endl);
          return navKeys(in.read());
        }
    };

  }

#endif
