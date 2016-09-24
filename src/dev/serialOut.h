#ifndef RSITE_ARDUINO_MENU_SERIALOUT
  #define RSITE_ARDUINO_MENU_SERIALOUT
  #include "../menu.h"

  namespace Menu {
    class serialOut:public menuOut {
      public:
        Print& device;
        inline serialOut(Print& o):device(o) {}
        virtual size_t write(uint8_t ch) {return device.write(ch);}
    };
  }//namespace Menu


#endif
