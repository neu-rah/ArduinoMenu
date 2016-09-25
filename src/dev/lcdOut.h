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
        void clearLine(int ln) override {
        	setCursor(0,ln);
        	for(int n=0;n<maxX;n++) print(' ');
        	setCursor(0,ln);
        }
        void clear() override {device.clear();}
        void setCursor(int x,int y) override {device.setCursor(x*resX,y*resY);}
        void printMenu(navNode &nav) override {
          Serial<<"lcd printMenu "<<nav.target->changed(nav,*this)<<endl;
          idx_t ot=nav.top;
          while(nav.sel>=nav.top+maxY) nav.top++;
          while(nav.sel<nav.top) nav.top--;
          if (drawn!=nav.target) {
            device.clear();
            drawn=nav.target;
          }
          if (nav.top!=ot||drawn!=nav.target||nav.target->changed(nav,*this)) {
            Serial<<"top:"<<nav.top<<" sel:"<<nav.sel<<endl;
            for(idx_t i=0;i<maxY;i++) {
              if (i+nav.top>=nav.sz()) break;
              Serial<<"printing "<<i+nav.top<<" "<<nav[i+nav.top]<<" at:"<<i<<endl;
              clearLine(i);
              setCursor(0,i);
              prompt& p=nav[i+nav.top];
              write(i+nav.top==nav.sel?options.selectedCursor:' ');
              p.printTo(i,nav,*this);
              p.dirty=false;
            }
            nav.target->dirty=false;
          }
        }
    };

  }//namespace Menu
#endif
