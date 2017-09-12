/* -*- C++ -*- */

//for using standard arduino LiquidCrystal driver

#ifndef RSITE_ARDUINO_MENU_LIQUIDCRYSTALOUT
  #define RSITE_ARDUINO_MENU_LIQUIDCRYSTALOUT
  #include "../menu.h"
  #include <LiquidCrystal.h>

  namespace Menu {

    class liquidCrystalOut:public cursorOut {
      public:
        LiquidCrystal& device;
        inline liquidCrystalOut(LiquidCrystal& o,idx_t *t,panelsList &p,menuOut::styles s=minimalRedraw)
          :cursorOut(t,p,s),device(o) {}
        size_t write(uint8_t ch) override {return device.write(ch);}
        void clear() override {
          device.clear();
          panels.reset();
        }
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          const panel p=panels[panelNr];
          device.setCursor(p.x+x,p.y+y);
        }
        virtual idx_t startCursor(bool charEdit) {return 0;}
        virtual idx_t endCursor(bool charEdit) {return 0;}
        virtual idx_t editCursor(idx_t x,idx_t y,bool editing,bool charEdit) {
          //text editor cursor
          device.noBlink();
          device.noCursor();
          if (editing) {
            device.setCursor(x, y);
            if (charEdit) device.cursor();
            else device.blink();
          }
          return 0;
        }
    };

  }//namespace Menu
#endif
