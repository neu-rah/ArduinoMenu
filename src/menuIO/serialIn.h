/* -*- C++ -*- */
/********************
Sept 2014 ~ Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
www.r-site.net

HardwareSerial input driver
***/

#ifndef RSITE_ARDUINO_MENU_HW_SERIALIN
  #define RSITE_ARDUINO_MENU_HW_SERIALIN
  #include "../menuDefs.h"

  #define SERIAL_CLASS Stream

  namespace Menu {
    class serialIn:public menuIn {
      public:
        Stream& in;
        serialIn(Stream& serial):in(serial) {}
        size_t write(uint8_t o) override {return in.write(o);}
        int available() override {return in.available();}
        int peek() override {return in.peek();}
        int read() override {return in.read();}
        void flush() override {in.flush();}
    };

  }

#endif
