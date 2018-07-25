/* -*- C++ -*- */

/********************
Sep. 2017 Rui Azevedo (ruihfazevedo@rrob@gmail.com)
Based on: Stephen Denne https://github.com/datacute U8G2 Driver

Use graphics screens as menu output, based on U8G2 graphic display
www.r-site.net
***/

#ifndef RSITE_ARDUINOP_MENU_UCGLIB
#define RSITE_ARDUINOP_MENU_UCGLIB
#include <Ucglib.h>
#include "menuDefs.h"

namespace Menu {

	struct rgb{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		inline rgb(uint8_t r,uint8_t g,uint8_t b):r(r),g(g),b(b) {}
	};

	class UCGLibOut:public gfxOut {
		public:
			Ucglib& gfx;
			int8_t offsetX=0;
			int8_t offsetY=0;
			const colorDef<rgb> (&colors)[nColors];
			UCGLibOut(
				Ucglib& gfx,
				const colorDef<rgb> (&c)[nColors],
				idx_t* t,
				panelsList &p,
				idx_t resX=6,
				idx_t resY=9,
				idx_t offsetX=0,
				idx_t offsetY=0
			) :gfxOut(resX,resY,t,p,menuOut::minimalRedraw),gfx(gfx),colors(c) {
					gfx.setFontPosBottom(); // U8Glib font positioning
					this->offsetX=offsetX;
					this->offsetY=offsetY;
			}

			size_t write(uint8_t ch) override {return gfx.write(ch);}

			inline rgb getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
				rgb* cor=(rgb*)&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]);
				return rgb(memByte(&cor->r),memByte(&cor->g),memByte(&cor->b));
			}

			void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false) override {
				rgb cor=getColor(c,selected,s,edit);
				gfx.setColor(cor.r,cor.g,cor.b);
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
				gfx.drawFrame((p.x+x)*resX,(p.y+y)*resY,w*resX,h*resY);
			}

			void rect(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
				const panel p=panels[panelNr];
				setColor(c,selected,stat,edit);
				gfx.drawBox((p.x+x)*resX,(p.y+y)*resY,w*resX,h*resY);
			}

			void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
				const panel p=panels[panelNr];
				gfx.setPrintPos((p.x+x)*resX + offsetX,(p.y+y+1)*resY-fontMarginY + offsetY);//+1 because font pos bottom
			}

			void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) override {
				const panel p=panels[panelNr];
				// gfxOut::drawCursor(ln,selected,stat);
				setColor(cursorColor,selected,stat);
				gfx.drawFrame(p.x*resX + offsetX ,(p.y+ln)*resY + offsetY ,maxX()*resX ,resY);
			}
	};
}
#endif //RSITE_ARDUINOP_MENU_U8G2
