/* -*- C++ -*- */

/***********
U8x8 Driver
Sep. 2017 Rui Azevedo (ruihfazevedo@rrob@gmail.com)
Based on: Stephen Denne https://github.com/datacute U8G2 Driver
*/

#ifndef RSITE_ARDUINO_MENU_U8X8
  #define RSITE_ARDUINO_MENU_U8X8
  #include "../menuDefs.h"
  #include <U8x8lib.h>

  namespace Menu {

    class U8x8Out:public cursorOut {
      public:
        U8X8& device;
        inline U8x8Out(U8X8& o,idx_t *t,panelsList &p,menuOut::styles s=minimalRedraw)
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
        virtual idx_t startCursor(bool charEdit) {device.setInverseFont(0);return 0;}
        virtual idx_t endCursor(bool charEdit) {device.setInverseFont(0);return 0;}
        virtual idx_t editCursor(idx_t x,idx_t y,bool editing,bool charEdit) {return 0;}
    };

  }//namespace Menu
#endif
