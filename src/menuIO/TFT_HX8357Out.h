/* -*- C++ -*- */
/********************
Feb.2018 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu with TFT_HX8357 driver https://github.com/Bodmer/TFT_HX8357
www.r-site.net

***/
#ifndef RSITE_TFT_HX8357_DEF_H
	#define RSITE_TFT_HX8357_DEF_H
  #include <TFT_HX8357.h>
	#include "../menuDefs.h"

namespace Menu {

		#define RGB565(r,g,b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

	  class tftHX8257Out:public gfxOut {
	    public:
				TFT_HX8357& gfx;
				const colorDef<uint16_t> (&colors)[nColors];
		    tftHX8257Out(TFT_HX8357& gfx,const colorDef<uint16_t> (&c)[nColors],idx_t* t,panelsList &p,idx_t resX=6,idx_t resY=9)
					:gfxOut(resX,resY,t,p),gfx(gfx),colors(c) {}
					//:gfxOut(gfx.width()/resX,gfx.height()/resY,resX,resY,false),colors(c),gfx(gfx) {}

				size_t write(uint8_t ch) override {return gfx.write(ch);}

				inline uint16_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
          return memWord(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
        }

				void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {
					gfx.setTextColor(getColor(c,selected,s,e));
				}

				void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					gfx.fillRect(p.x*resX,(p.y+ln)*resY,p.maxX()*resX,resY,getColor(color,selected,stat,edit));
		    	//setCursor(0,ln);
		    }
		    void clear() override {
					panels.reset();
					gfx.fillScreen(getColor(bgColor,false,enabledStatus,false));
		    	setCursor(0,0);
					setColor(fgColor);
				}

				void box(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					gfx.drawRect((p.x+x)*resX,(p.y+y)*resY,w*resX,h*resY,getColor(c,selected,stat,edit));
				}

				void rect(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
					const panel p=panels[panelNr];
					gfx.fillRect((p.x+x)*resX,(p.y+y)*resY,w*resX,h*resY,getColor(c,selected,stat,edit));
				}

				void clear(idx_t panelNr) override {
					const panel p=panels[panelNr];
					gfx.fillRect(p.x*resX,p.y*resY,p.w*resX,p.h*resY,getColor(bgColor,false,enabledStatus,false));
					panels.nodes[panelNr]=NULL;
				}

		    void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
					const panel p=panels[panelNr];
					gfx.setCursor((p.x+x)*resX,(p.y+y)*resY+fontMarginY);
				}

				void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) override {
					const panel p=panels[panelNr];
					// gfxOut::drawCursor(ln,selected,stat);
					gfx.drawRect(p.x*resX,(p.y+ln)*resY,maxX()*resX,resY,getColor(cursorColor,selected,enabledStatus,false));
				}
	  };

}; //namespace adaGfxOut
#endif
