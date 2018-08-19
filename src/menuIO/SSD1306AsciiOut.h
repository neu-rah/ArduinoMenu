/* -*- C++ -*- */

//using SSD1306Ascii
//https://github.com/greiman/SSD1306Ascii

#ifndef RSITE_ARDUINO_MENU_SSD1306ASCIIOUT
  #define RSITE_ARDUINO_MENU_SSD1306ASCIIOUT

  #include "../menuDefs.h"
  // #include <Wire.h>
  // #include <LCD.h>

  namespace Menu {

    class SSD1306AsciiOut:public gfxOut {
      public:
        SSD1306Ascii* device;
        SSD1306AsciiOut(
          SSD1306Ascii* o,
  				idx_t* t,
  				panelsList &p,
  				idx_t resX=6,
  				idx_t resY=1,
          menuOut::styles s=menuOut::minimalRedraw
  			):gfxOut(resX,resY,t,p,s),device(o) {}
        void init() {
          resX=device->fontHeight();
          resY=device->fontWidth();
          fontMarginX=0;
          fontMarginY=0;
        }
        size_t write(uint8_t ch) override {return device->write(ch);}
        void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
          const panel p=panels[panelNr];
          int y=(p.y+ln)*resY;
          device->clear(resX*p.x,resX*(p.x+maxX(panelNr))-1,y,y+resY-1);
        }
        void clear() override {
          trace(Serial<<"clear!!"<<endl;);
          device->clear();
          panels.reset();
        }
        void clear(idx_t panelNr) override {
          const panel p=panels[panelNr];
          trace(Serial<<"clear panel!! "<<panelNr<<"{"<<p.x<<","<<p.y<<","<<p.w<<","<<p.h<<"}"<<endl;);
          device->clear(resX*p.x,resX*(p.x+p.w)-1,resY*p.y,resY*(p.y+p.h)-1);
          // setCursor(0,0,panelNr);
          panels.nodes[panelNr]=NULL;
        }
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          const panel p=panels[panelNr];
          device->setCursor((p.x+x)*resX,(p.y+y)*resY);
        }
    };

  }//namespace Menu

#endif
