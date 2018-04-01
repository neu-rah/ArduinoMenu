/* -*- C++ -*- */
/********************
Sept 2014 ~ Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
www.r-site.net

HardwareSerial input driver
***/

#ifndef RSITE_ARDUINO_MENU_HW_SERIALIN
  #define RSITE_ARDUINO_MENU_HW_SERIALIN
  #include "../menu.h"

  #if defined(CORE_TEENSY)
    #define SERIAL_CLASS usb_serial_class
  #else
    #define SERIAL_CLASS HardwareSerial
  #endif

  namespace Menu {
    class serialIn:public streamIn {
      protected:
        SERIAL_CLASS& in;
      public:
        serialIn(SERIAL_CLASS& serial):in(serial) {}
        // size_t write(uint8_t o) override {return in.write(o);}
        int available() override {return in.available()?*this:false;}
        navCmds peek() override {return navKeys(in.peek());}
        navCmds getCmd() override {return navKeys(in.read());}
        // void flush() override {in.flush();}
    };

  }

#endif
