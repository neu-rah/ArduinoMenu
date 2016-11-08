/* -*- C++ -*- */
//for using standard arduino LiquidCrystal driver

#ifndef RSITE_ARDUINO_MENU_LIQUIDCRYSTALOUT
  #define RSITE_ARDUINO_MENU__LIQUIDCRYSTALOUT
  #include "../menu.h"
  #include <LiquidCrystal.h>

  namespace Menu {

    class liquidCrystalOut:public menuOut {
      public:
        LiquidCrystal& device;
        inline liquidCrystalOut(LiquidCrystal& o,idx_t *t,panelsList &p,menuOut::styles s=minimalRedraw)
          :menuOut(t,p,s),device(o) {}
        size_t write(uint8_t ch) override {return device.write(ch);}
        void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
        	setCursor(0,ln,panelNr);
        	for(int n=0;n<maxX();n++) print(' ');
        	setCursor(0,ln,panelNr);
        }
        void clear() override {
          device.clear();
          panels.reset();
        }
        void clear(idx_t panelNr) override {
          const panel p=panels[panelNr];
          //device<<endl;
          fill(p.x,p.y,p.x+p.w-1,p.y+p.h-1);
          setCursor(0,0,panelNr);
          panels.nodes[panelNr]=NULL;
        }
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          const panel p=panels[panelNr];
          device.setCursor(p.x+x,p.y+y);
        }
    };

  }//namespace Menu
#endif
