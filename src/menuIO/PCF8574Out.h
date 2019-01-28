/* -*- C++ -*- */

//using Matthias Hertel driver https://github.com/mathertel/LiquidCrystal_PCF8574

#ifndef RSITE_ARDUINO_MENU_PCF8574OUT
  #define RSITE_ARDUINO_MENU_PCF8574OUT

  // #ifndef ARDUINO_SAM_DUE
    #include "../menuDefs.h"
    #include <Wire.h>
    #include <LiquidCrystal_PCF8574.h>

    namespace Menu {

      class lcdOut:public cursorOut {
        public:
          LiquidCrystal_PCF8574* device;
          inline lcdOut(LiquidCrystal_PCF8574* o,idx_t *t,panelsList &p,menuOut::styles s=menuOut::minimalRedraw)
            :cursorOut(t,p,s),device(o) {}
          size_t write(uint8_t ch) override {return device->write(ch);}
          void clear() override {
            device->clear();
            panels.reset();
          }
          void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
            const panel p=panels[panelNr];
            device->setCursor(p.x+x,p.y+y);
          }
          idx_t startCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr=0) override {return 0;}
          idx_t endCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr=0) override {return 0;}
          idx_t editCursor(navRoot& root,idx_t x,idx_t y,bool editing,bool charEdit,idx_t panelNr=0) override {
            trace(MENU_DEBUG_OUT<<"lcdOut::editCursor "<<x<<","<<y<<endl);
            //text editor cursor
            device->noBlink();
            device->noCursor();
            if (editing) {
              device->setCursor(x, y);
              if (charEdit) device->cursor();
              else device->blink();
            }
            return 0;
          }

      };

    }//namespace Menu

  #endif
// #endif
