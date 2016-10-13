#ifndef RSITE_ARDUINO_MENU_LCDOUT
  #define RSITE_ARDUINO_MENU_LCDOUT
  #include "../menu.h"
  #include <LiquidCrystal.h>

  namespace Menu {

    class lcdOut:public menuOut {
      public:
        LiquidCrystal& device;
        inline lcdOut(LiquidCrystal& o,int x,int y):menuOut(x,y),device(o) {}
        size_t write(uint8_t ch) override {return device.write(ch);}
        void clearLine(idx_t ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
        	setCursor(0,ln);
        	for(int n=0;n<maxX;n++) print(' ');
        	setCursor(0,ln);
        }
        void clear() override {device.clear();}
        void setCursor(idx_t x,idx_t y) override {device.setCursor(x,y);}
    };

  }//namespace Menu
#endif
