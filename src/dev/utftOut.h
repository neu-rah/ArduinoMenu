/* -*- C++ -*- */
/********************
Use UTFT graphics display as menu output

www.r-site.net

Dec. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

UTFT library from:
  http://www.rinkydinkelectronics.com/library.php?id=51
  http://henningkarlsen.com/electronics/library.php?id=50 (old address)

***/
#ifndef RSITE_ARDUINO_MENU_UTFT
	#define RSITE_ARDUINO_MENU_UTFT
	#include <UTFT.h>
	#include "../menu.h"

  namespace Menu {

    #define RGB565(r,g,b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

    class menuUTFT:public gfxOut {
      public:
        UTFT& gfx;
        const colorDef<uint16_t> (&colors)[nColors];
        idx_t curX,curY;
        menuUTFT(UTFT& tft,const colorDef<uint16_t> (&c)[nColors],panelsList &p,idx_t resX,idx_t resY)
          :gfxOut(resX,resY,p,false),gfx(tft),colors(c),curX(0),curY(0) {}
        /*void init() {
          //setup geometry
          resX=gfx.getFontXsize();
          resY=gfx.getFontYsize();
          //maxX=gfx.getDisplayXSize()/resX;
          //maxY=gfx.getDisplayYSize()/resY;
        }*/

        inline uint16_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
          return memWord(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
        }

        void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {
          gfx.setColor(getColor(c,selected,s,e));
				}

        void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					uint16_t c=getColor(color,selected,stat,edit);
          gfx.setColor(c);
					gfx.fillRect(p.x*resX,(p.y+ln)*resY,(p.x+p.maxX())*resX,(p.y+ln+1)*resY);
					gfx.setBackColor(c);
		    	//setCursor(0,ln);
		    }

        void clear() override {
					panels.reset();
          gfx.setBackColor(getColor(bgColor,false,enabledStatus,false));
          gfx.clrScr();
					curX=curY=0;
				}

        virtual void clear(idx_t panelNr) override {
					const panel p=panels[panelNr];
          gfx.setColor(getColor(bgColor,false,enabledStatus,false));
					gfx.fillRect(p.x*resX,p.y*resY,(p.x+p.w)*resX,(p.y+p.h)*resY);
					panels.nodes[panelNr]=NULL;
				}

        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          const panel p=panels[panelNr];
    	    curX=(p.x+x)*resX;
    	    curY=(p.y+y)*resY+fontMarginY;
    	  }
        size_t write(uint8_t ch) override {
          switch(ch) {
          case 0x0D://NL
            curX=0;
            break;
          case 0x0A://CR
            curY+=resY;
            break;
          default:
            gfx.printChar(ch,curX,curY);
            curX+=resX;
          }
          return 1;
        }
    };

  }//namespace Menu
#endif
