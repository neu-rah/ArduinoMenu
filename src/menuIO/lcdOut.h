/* -*- C++ -*- */

//for using F. Malpartida LCD's driver
//https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

#ifndef RSITE_ARDUINO_MENU_LCDOUT
  #define RSITE_ARDUINO_MENU_LCDOUT

  #ifndef ARDUINO_SAM_DUE
    #include "../menu.h"
    #include <Wire.h>
    #include <LCD.h>

    namespace Menu {

      class lcdOut:public cursorOut {
        public:
          LCD* device;
          inline lcdOut(LCD* o,idx_t *t,panelsList &p,menuOut::styles s=menuOut::minimalRedraw)
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
          menuOut& fill(
            int x1, int y1, int x2, int y2,char ch=' ',
            colorDefs color=bgColor,
            bool selected=false,
            status stat=enabledStatus,
            bool edit=false
          ) override {
            for(int r=y1;r<=y2;r++) {
              setCursor(x1,r);
              for(int c=x1;c<=x2;c++)
                write(ch);
            }
            return *this;
          }

      };

    }//namespace Menu

  #endif
#endif
