/* -*- C++ -*- */

/********************
Oct. 2016 Stephen Denne https://github.com/datacute
Based on U8GLibOut.h from Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz

Use graphics screens as menu output, based on U8G2 graphic display
www.r-site.net

***/
#ifndef RSITE_ARDUINOP_MENU_U8G2
#define RSITE_ARDUINOP_MENU_U8G2
#include <U8g2lib.h>
#include "menu.h"

namespace Menu {

	class u8g2Out:public gfxOut {
		public:
			U8G2& gfx;
			//int8_t fontMarginY=1;
			int8_t offsetX=0;
			int8_t offsetY=0;
			const colorDef<uint8_t> (&colors)[nColors];
			u8g2Out(
				U8G2& gfx,
				const colorDef<uint8_t> (&c)[nColors],
				idx_t* t,
				panelsList &p,
				idx_t resX=6,
				idx_t resY=9,
				idx_t offsetX=0,
				idx_t offsetY=0
			) :gfxOut(resX,resY,t,p,(styles)(menuOut::redraw|menuOut::rasterDraw)),gfx(gfx),colors(c) {
					gfx.setFontPosBottom(); // U8Glib font positioning
					this->offsetX=offsetX;
					this->offsetY=offsetY;
			}

			size_t write(uint8_t ch) override {return gfx.write(ch);}

			inline uint8_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
				return memByte(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
			}

			void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false) override {
				gfx.setDrawColor(getColor(c,selected,s,edit));
			}

			void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
				const panel p=panels[panelNr];
				setColor(color,selected,stat,edit);
				gfx.drawBox(p.x*resX + offsetX - 1,(p.y+ln)*resY + offsetY /*- fontMarginY*/,maxX()*resX /*+ fontMarginY*/ /*+ fontMarginY*/,resY /*+ fontMarginY*/ /*+ fontMarginY*/);
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
				gfx.drawBox(p.x*resX + offsetX,p.y*resY + offsetY,p.w*resX,p.h*resY);
				//clear();
				panels.nodes[panelNr]=NULL;
			}
			void box(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
				const panel p=panels[panelNr];
				gfx.drawFrame((p.x+x)*resX,(p.y+y-1)*resY,w*resX,h*resY/*+(fontMarginY<<1)*/);
			}

			void rect(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
				const panel p=panels[panelNr];
				gfx.drawBox((p.x+x)*resX,(p.y+y-1)*resY,w*resX,h*resY/*+(fontMarginY<<1)*/);
			}

			void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
				const panel p=panels[panelNr];
				gfx.tx = (p.x+x)*resX + offsetX;
				gfx.ty = (p.y+y+1)*resY-fontMarginY + offsetY;
			}

			void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) override {
				const panel p=panels[panelNr];
				gfxOut::drawCursor(ln,selected,stat);
				setColor(cursorColor,selected,stat);
				gfx.drawFrame(p.x*resX + offsetX /*- fontMarginY*/,(p.y+ln)*resY + offsetY /*- fontMarginY*/,maxX()*resX /*+ fontMarginY*/ /*+ fontMarginY*/,resY /*+ fontMarginY*/ /*+ fontMarginY*/);
			}
	};
}
#endif //RSITE_ARDUINOP_MENU_U8G2
