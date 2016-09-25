#ifndef RSITE_ARDUINO_MENU_LCDOUT
  #define RSITE_ARDUINO_MENU_LCDOUT
  #include "../menu.h"
  #include <LiquidCrystal.h>

  namespace Menu {

    class lcdOut:public menuOut {
      public:
        LiquidCrystal& device;
        inline lcdOut(LiquidCrystal& o):device(o) {}
        size_t write(uint8_t ch) override {return device.write(ch);}
        virtual void setCursor(int x,int y) {device.setCursor(x*resX,y*resY);}
        virtual void printMenu(navNode &nav) {
          if (nav.target->changed(nav,*this)) {
            //*this<<"["<<*(prompt*)nav.target<<"]"<<endl;
            for(idx_t i=0;i<maxY;i++) {
              setCursor(0,i);
              if (i+nav.top>=nav.sz()) break;
              prompt& p=nav[i+nav.top];
              write(i+nav.top==nav.sel?options.selectedCursor:' ');
              p.printTo(i,nav,*this);
              //*this<<endl;
              p.dirty=false;
            }
            nav.target->prompt::dirty=false;
          }
        }
    };

  }//namespace Menu
#endif
