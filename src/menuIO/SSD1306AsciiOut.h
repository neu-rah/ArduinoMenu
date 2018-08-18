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
          _trace(MENU_DEBUG_OUT<<"ln "<<ln<<endl);
          _trace(MENU_DEBUG_OUT<<"p.y "<<p.y<<endl);
          _trace(MENU_DEBUG_OUT<<"resY "<<resY<<endl);
          _trace(MENU_DEBUG_OUT<<"fontHeight() "<<((device->fontHeight()-1)>>3)<<endl);
          // int h=resY*(((device->fontHeight()-1)>>3)+1);
          // _trace(MENU_DEBUG_OUT<<"h "<<h<<endl);
          int y=(p.y+ln)*resY;
          _trace(Serial<<"clearing line "<<y<<","<<(y+resY-1)<<endl;);
          device->clear(resX*p.x,resX*(p.x+maxX(panelNr))-1,y,y+resY-1);
          // setCursor(0,ln,panelNr);
        	// for(int n=0;n<maxX();n++) print(' ');
        	// setCursor(0,ln,panelNr);
        }
        void clear() override {
          _trace(Serial<<"clear!!"<<endl;);
          device->clear();
          panels.reset();
        }
        void clear(idx_t panelNr) override {
          _trace(Serial<<"clear panel!! "<<panelNr<<endl;);
          const panel p=panels[panelNr];
          device->clear(p.x,p.y,p.x+p.w-1,p.y+p.h-1);
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
