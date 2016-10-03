/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

Use graphics screens (adafruit library based) as menu output
www.r-site.net
***/
#ifndef RSITE_ARDUINOP_MENU_GFX
	#define RSITE_ARDUINOP_MENU_GFX
	#include <Adafruit_GFX.h>
	#include "../menu.h"

namespace Menu {

		#define RGB565(r,g,b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

		// Color definitions RGB565
		#define BLACK 0x0000
		#define BLUE 0x001F
		#define RED 0xF800
		#define GREEN 0x07E0
		#define GRAY RGB565(128,128,128)
		#define SILVER RGB565(200,200,200)
		#define CYAN 0x07FF
		#define MAGENTA 0xF81F
		#define YELLOW 0xFFE0
		#define WHITE 0xFFFF

	  class menuGFX:public gfxOut {
	    public:
				Adafruit_GFX& gfx;
		    menuGFX(Adafruit_GFX& gfx,int resX=6,int resY=9)
			  	:gfxOut(gfx.width()/resX,gfx.height()/resY,resX,resY),gfx(gfx) {}

		    void clearLine(int ln) override {
		    	//gfx.fillRect(0,ln*resY,resX*maxX,resY,bgColor);
		    	setCursor(0,ln);
		    }
		    void clear() override {
		    	//gfx.fillRect(0,0,resX*maxX,resY*maxY,bgColor);
		    	setCursor(0,0);
		    }
		    void setCursor(int x,int y) override {gfx.setCursor(x*resX,y*resY);}
		    size_t write(uint8_t ch) override {return gfx.write(ch);}
		    //void printPrompt(prompt &o,bool selected,int idx,int posX,int posY,int width) override {
		    	/*gfx.fillRect(0,posY*resY,maxX*resX,resY,selected?hiliteColor:bgColor);
		    	gfx.setTextColor(selected?(o.enabled?enabledColorHi:disabledColorHi):(o.enabled?enabledColor:disabledColor));
		    	setCursor(0,posY);
		    	o.printTo(*(menuOut*)this);*/
		    //}
				void printMenu(navNode &nav) override {
					/*if (drawn!=&m) clear();//clear all screen when changing menu
					if (m.sel-top>=maxY) top=m.sel-maxY+1;//selected option outside device (bottom)
					else if (m.sel<top) top=m.sel;//selected option outside device (top)
					int i=top;for(;i<m.sz;i++) {
						  if(i-top>=maxY) break;
						  if (needRedraw(m,i)) {
						  	printPrompt(*(prompt*)pgmPtrNear(m.data[i]),i==m.sel,i+1,0,i-top,m.width);
						  }
					}
					if (drawExit&&i-top<maxY&&needRedraw(m,i))
						printPrompt(menu::exitOption,m.sel==m.sz,0,0,i-top,m.width);
					lastTop=top;
					lastSel=m.sel;
					drawn=&m;*/
				}
				void clearChanged(navNode &nav) override {}
	  };

}; //namespace menuGFX
#endif
