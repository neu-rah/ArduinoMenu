/* -*- C++ -*- */
/********************
Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

Use graphics screens as menu output, based on U8GLIB graphic display
www.r-site.net

 printPrompt(), Added posX to take into account ox via menu.ox and setPosition(ox,oy), need to update other files to support that.

***/
#ifndef RSITE_ARDUINOP_MENU_U8G
	#define RSITE_ARDUINOP_MENU_U8G
  #include <U8glib.h>
	#include "menu.h"

  namespace Menu {

    class u8gLibOut:public gfxOut {
      public:
	      U8GLIB& gfx;
				int8_t fontMargin=2;
				const colorDef<uint8_t> (&colors)[nColors];
	      u8gLibOut(
					U8GLIB& gfx,
					const colorDef<uint8_t> (&c)[nColors],
					idx_t* t,
					panelsList &p,
					idx_t resX=6,
					idx_t resY=9
				) :gfxOut(resX,resY,t,p,menuOut::redraw),gfx(gfx),colors(c) {
	        	gfx.setFontPosBottom(); // U8Glib font positioning
	      }

				size_t write(uint8_t ch) override {
					switch(ch) {//fix u8glib not respecting \n\r... add \t if you wish
						case '\n': gfx.setPrintPos(gfx.getPrintCol(), gfx.getPrintRow()+resY-fontMargin);break;
						case '\r': gfx.setPrintPos(0, gfx.getPrintRow());break;
						default: return gfx.write(ch);
					}
					return 1;
	      }

				inline uint8_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
          return memByte(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
        }

				void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false) override {
					gfx.setColorIndex(getColor(c,selected,s,edit));
				}

				void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					setColor(color,selected,stat,edit);
					gfx.drawBox(p.x*resX,(p.y+ln)*resY,maxX()*resX,resY);
          //setCursor(0,ln);
	      }
	      void clear() override {
					setCursor(0,0);
					setColor(fgColor);
					panels.reset();
	      }
				void clear(idx_t panelNr) override {
					const panel p=panels[panelNr];
					setColor(bgColor,false,enabledStatus,false);
					gfx.drawBox(p.x*resX,p.y*resY,p.w*resX,p.h*resY);
					//clear();
					panels.nodes[panelNr]=NULL;
				}
	      void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
					const panel p=panels[panelNr];
          gfx.setPrintPos((p.x+x)*resX,(p.y+y+1)*resY-fontMargin);
	      }

				void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) override {
					const panel p=panels[panelNr];
					gfxOut::drawCursor(ln,selected,stat);
					setColor(cursorColor,selected,stat);
					gfx.drawFrame(p.x*resX,(p.y+ln)*resY,maxX()*resX,resY);
				}
  	};
}
#endif //RSITE_ARDUINOP_MENU_LCD
