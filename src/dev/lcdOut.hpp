#ifndef RSITE_ARDUINO_MENU_LCDOUT
  #define RSITE_ARDUINO_MENU_LCDOUT
  #include "../menu.hpp"
  #include <LiquidCrystal.h>

  namespace Menu {

    class lcdOut:public menuOut {
      public:
        LiquidCrystal& device;
        inline lcdOut(LiquidCrystal& o,panelsList &p,bool r=false,bool m=true)
          :menuOut(p,r,m),device(o) {}
        size_t write(uint8_t ch) override {return device.write(ch);}
        void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
        	setCursor(0,ln,panelNr);
        	for(int n=0;n<maxX();n++) print(' ');
        	setCursor(0,ln,panelNr);
        }
        void clear() override {device.clear();}
        void clear(idx_t panelNr) override {device<<endl;}
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          const panel p=panels[panelNr];
          device.setCursor(p.x+x,p.y+y);
        }
    };

  }//namespace Menu
#endif
