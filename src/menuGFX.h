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
	#include "menu.h"

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

  class menuGFX:public menuOut {
    public:
  	uint16_t hiliteColor;
  	uint16_t bgColor;
  	uint16_t enabledColor;
  	uint16_t disabledColor;
		uint16_t enabledColorHi;
		uint16_t disabledColorHi;
    Adafruit_GFX& gfx;
    menuGFX(
    	Adafruit_GFX& gfx,
    	uint16_t hiliteColor=BLUE,
    	uint16_t bgColor=BLACK,
			uint16_t enabledColor=WHITE,
    	uint16_t disabledColor=SILVER,
    	int resX=6,
    	int resY=9
    )
	  	:gfx(gfx),
	  	bgColor(bgColor),
	  	enabledColor(enabledColor),
			disabledColor(disabledColor),
	  	hiliteColor(hiliteColor),
			enabledColorHi(bgColor),
			disabledColorHi(bgColor),
	  	menuOut(gfx.width()/resX,gfx.height()/resY,resX,resY) {}

    virtual void clearLine(int ln) {
    	gfx.fillRect(0,ln*resY,resX*maxX,resY,bgColor);
    	setCursor(0,ln);
    }
    virtual void clear() {
    	gfx.fillRect(0,0,resX*maxX,resY*maxY,bgColor);
    	setCursor(0,0);
    }
    virtual void setCursor(int x,int y) {gfx.setCursor(x*resX,y*resY);}
    virtual size_t write(uint8_t ch) {return gfx.write(ch);}
    virtual void printPrompt(prompt &o,bool selected,int idx,int posX,int posY,int width) {
    	gfx.fillRect(0,posY*resY,maxX*resX,resY,selected?hiliteColor:bgColor);
    	gfx.setTextColor(selected?(o.enabled?enabledColorHi:disabledColorHi):(o.enabled?enabledColor:disabledColor));
    	setCursor(0,posY);
    	o.printTo(*this);
    }
		virtual void printMenu(menu& m,bool drawExit) {
			if (drawn!=&m) clear();//clear all screen when changing menu
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
			drawn=&m;
		}
  };
#endif RSITE_ARDUINOP_MENU_LCD
