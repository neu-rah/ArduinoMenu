/* -*- C++ -*- */

/********************
Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz

Use graphics screens as menu output, based on U8GLIB graphic display
www.r-site.net

 printPrompt(), Added posX to take into account ox via menu.ox and setPosition(ox,oy), need to update other files to support that.

***/
#ifndef RSITE_ARDUINOP_MENU_U8G
	#define RSITE_ARDUINOP_MENU_U8G
  #include <U8glib.h>
	#include "menuDefs.h"

  namespace Menu {

    class u8gLibOut:public gfxOut {
      public:
	      U8GLIB& gfx;
				//int8_t fontMarginY=1;
				const colorDef<uint8_t> (&colors)[nColors];
	      u8gLibOut(
					U8GLIB& gfx,
					const colorDef<uint8_t> (&c)[nColors],
					idx_t* t,
					panelsList &p,
					idx_t resX=6,
					idx_t resY=9
				) :gfxOut(resX,resY,t,p,(menuOut::styles)(menuOut::redraw|menuOut::rasterDraw)),gfx(gfx),colors(c) {
	        	gfx.setFontPosBottom(); // U8Glib font positioning
	      }

				size_t write(uint8_t ch) override {return gfx.write(ch);}
				inline uint8_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
          return memByte(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
        }

				void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false) override {
					gfx.setColorIndex(getColor(c,selected,s,edit));
				}

				void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					setColor(color,selected,stat,edit);
					gfx.drawBox(p.x*resX,(p.y+ln)*resY,maxX()*resX,resY/*+(fontMarginY<<1)*/);
	      }
	      void clear() override {
					setCursor(0,0);
					setColor(fgColor);
					panels.reset();
	      }
				void clear(idx_t panelNr) override {
					const panel p=panels[panelNr];
					setColor(bgColor,false,enabledStatus,false);
					gfx.drawBox(p.x*resX,p.y*resY,p.w*resX,p.h*resY/*+(fontMarginY<<1)*/);
					panels.nodes[panelNr]=NULL;
				}
				void box(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					setColor(c,selected,stat,edit);
					gfx.drawFrame((p.x+x)*resX,(p.y+y-1)*resY,w*resX,h*resY/*+(fontMarginY<<1)*/);
				}

				void rect(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					setColor(c,selected,stat,edit);
					gfx.drawBox((p.x+x)*resX,(p.y+y-1)*resY,w*resX,h*resY/*+(fontMarginY<<1)*/);
				}

	      void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
					const panel p=panels[panelNr];
          gfx.setPrintPos((p.x+x)*resX,(p.y+y+1)*resY-fontMarginY);
	      }

				void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) override {
					const panel p=panels[panelNr];
					// gfxOut::drawCursor(ln,selected,stat);
					// if (stat==disabledStatus) {
					setColor(cursorColor,selected,stat);
					// 	gfx.drawFrame(p.x*resX,(p.y+ln)*resY,maxX()*resX,resY/*+(fontMarginY<<1)*/);
					// }
          gfx.drawFrame(p.x*resX /*- fontMarginY*/,(p.y+ln)*resY /*- fontMarginY*/,maxX()*resX /*+ fontMarginY*/ /*+ fontMarginY*/,resY /*+ fontMarginY*/ /*+ fontMarginY*/);
				}
  	};
}
#endif //RSITE_ARDUINOP_MENU_LCD
