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
#include "menuDefs.h"

namespace Menu {

	class u8g2Out:public gfxOut {
		public:
      int8_t offsetX=0;
      int8_t offsetY=0;
			U8G2& gfx;
			const colorDef<uint8_t> (&colors)[nColors];
			u8g2Out(
				U8G2& gfx,
				const colorDef<uint8_t> (&c)[nColors],
				idx_t* t,
				panelsList &p,
				idx_t resX=6,
				idx_t resY=9,
				idx_t offsetX=0,
				idx_t offsetY=0,
        int fontMarginY=1
			) :gfxOut(resX,resY,t,p,(styles)(menuOut::redraw|menuOut::rasterDraw)),gfx(gfx),colors(c) {
					gfx.setFontPosBottom(); // U8Glib font positioning
					this->offsetX=offsetX;
					this->offsetY=offsetY;
          this->fontMarginY=fontMarginY;
			}

			u8g2Out(
				U8G2& gfx,
				const colorDef<uint8_t> (&c)[nColors],
				idx_t* t,
				panelsList &p,
				idx_t resX,
				idx_t resY,
				idx_t offsetX,
				idx_t offsetY,
				int fontMarginX,
				int fontMarginY
			) :gfxOut(resX,resY,t,p,(styles)(menuOut::redraw|menuOut::rasterDraw)),gfx(gfx),colors(c) {
					gfx.setFontPosBottom(); // U8Glib font positioning
					this->offsetX=offsetX;
					this->offsetY=offsetY;
					this->fontMarginX=fontMarginX;
					this->fontMarginY=fontMarginY;
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
				gfx.drawBox(p.x*resX + offsetX /*- 1*/,(p.y+ln)*resY + offsetY /*- fontMarginY*/,maxX()*resX /*+ fontMarginY*/ /*+ fontMarginY*/,resY /*+ fontMarginY*/ /*+ fontMarginY*/);
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
				gfx.drawFrame((p.x+x)*resX,(p.y+y)*resY,w*resX,h*resY/*+(fontMarginY<<1)*/);
			}

			void rect(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
				const panel p=panels[panelNr];
				gfx.drawBox((p.x+x)*resX,(p.y+y)*resY,w*resX,h*resY/*+(fontMarginY<<1)*/);
			}

			void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
				// _trace(Serial<<"setCursor"<<endl);
				const panel p=panels[panelNr];
				gfx.tx = (p.x+x)*resX+fontMarginX + offsetX;
				gfx.ty = (p.y+y+1)*resY-fontMarginY + offsetY;
			}

			idx_t startCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr) override {
			  if (charEdit) {
			    // rect(panelNr,  x-1,  y, 1, 1, bgColor, false, enabledStatus, false);
					const panel p=panels[panelNr];
					gfx.drawBox((p.x+x)*resX + offsetX +fontMarginX,(p.y+y)*resY+ offsetY /*-fontMarginY*/,resX,resY);
			    setColor(fgColor,false,enabledStatus,false);
			  }/* else
			    box(panelNr,  x,  y, 1, 1, bgColor, false, enabledStatus, false);*/
			  return 0;
			}

			// idx_t endCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr) override {
			//   setColor(fgColor,true,enabledStatus,true);return 0;
			// }

      idx_t editCursor(navRoot& root,idx_t x,idx_t y,bool editing,bool charEdit,idx_t panelNr=0) override {
			  if (editing) {
					_trace(Serial<<"editCursor"<<endl);
					// box(panelNr,x-1,y);
					const panel p=panels[panelNr];
					gfx.drawFrame((x+p.x-1)*resX + offsetX + fontMarginX-1,(p.y+y)*resY + offsetY /*- fontMarginY+2*/,resX+1 ,resY);
				}
			  return 0;
			}

			void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) override {
				// _trace(Serial<<"drawCursor"<<endl);
				const panel p=panels[panelNr];
				// gfxOut::drawCursor(ln,selected,stat);
				setColor(cursorColor,selected,stat);
				gfx.drawFrame(p.x*resX + offsetX /*+ fontMarginX*/,(p.y+ln)*resY + offsetY /*+ fontMarginY*/,maxX()*resX ,resY);
			}
			idx_t printRaw(const char* at,idx_t len) override {
				trace(Serial<<"u8g2Out::printRaw"<<endl);
				trace(Serial<<"["<<at<<"]");
				return print((__FlashStringHelper*)at);
			  // const char* p=at;
			  // uint8_t ch;
			  // for(int n=0;(ch=memByte(at++))&&(len==0||n<len);n++) {
				// 	trace(Serial<<"["<<ch<<"]");
			  //   write(ch);
			  // }
			  // return at-p-1;
			}

	};
}
#endif //RSITE_ARDUINOP_MENU_U8G2
