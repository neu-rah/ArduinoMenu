/* -*- C++ -*- */

//using OzOled
//using https://github.com/kdkanishka/ozoled-oled-display-096-i2c-driver-library

#ifndef RSITE_ARDUINO_MENU_OZOLEDASCIIOUT
#define RSITE_ARDUINO_MENU_OZOLEDASCIIOUT

  #include "../menuDefs.h"
  // #include <Wire.h>
  // #include <LCD.h>

  namespace Menu {

    class OzOledAsciiOut:public menuOut {
      public:
        OzOLED* device;
        inline OzOledAsciiOut(OzOLED* o,idx_t *t,panelsList &p,menuOut::styles s=menuOut::minimalRedraw)
          :menuOut(t,p,s),device(o) {}
        size_t write(uint8_t ch) override { device->printChar(ch); return 1;}
        void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
        	setCursor(0,ln,panelNr);
        	for(int n=0;n<maxX();n++) print(' ');
        	setCursor(0,ln,panelNr);
        }
        void clear() override {
          device->clearDisplay();
          panels.reset();
        }
        void clear(idx_t panelNr) override {
          const panel p=panels[panelNr];
          fill(p.x,p.y,p.x+p.w-1,p.y+p.h-1);
          setCursor(0,0,panelNr);
          panels.nodes[panelNr]=NULL;
        }
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          const panel p=panels[panelNr];
          device->setCursorXY(p.x+x,p.y+y);
        }
    };

  }//namespace Menu

#endif
