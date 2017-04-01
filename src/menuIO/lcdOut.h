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

      class lcdOut:public menuOut {
        public:
          LCD* device;
          inline lcdOut(LCD* o,idx_t *t,panelsList &p,menuOut::styles s=menuOut::minimalRedraw)
            :menuOut(t,p,s),device(o) {}
          size_t write(uint8_t ch) override {return device->write(ch);}
          void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
          	setCursor(0,ln,panelNr);
          	for(int n=0;n<maxX();n++) print(' ');
          	setCursor(0,ln,panelNr);
          }
          void clear() override {
            Serial.println("Clearing LCD");
            device->clear();
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
