/* -*- C++ -*- */

//for using I2C LiquidCrystal Library provided by
// https://gitlab.com/tandembyte/LCD_I2C

#ifndef RSITE_ARDUINO_MENU_LIQUIDCRYSTALOUT
  #define RSITE_ARDUINO_MENU_LIQUIDCRYSTALOUT
  #include "../menuDefs.h"
  #include <LiquidCrystal_I2C.h>

  namespace Menu {

    class liquidCrystalOut:public cursorOut {
      public:
        LiquidCrystal_I2C& device;
        inline liquidCrystalOut(LiquidCrystal_I2C& o,idx_t *t,panelsList &p,menuOut::styles s=minimalRedraw)
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
        idx_t startCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr=0) override {return 0;}
        idx_t endCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr=0) override {return 0;}
        idx_t editCursor(navRoot& root,idx_t x,idx_t y,bool editing,bool charEdit,idx_t panelNr=0) override {
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
