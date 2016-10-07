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
				const uint16_t (&colors)[nColors][2][2];
		    menuGFX(Adafruit_GFX& gfx,const uint16_t (&c)[nColors][2][2],int resX=6,int resY=9)
			  	:gfxOut(gfx.width()/resX,gfx.height()/resY,resX,resY),colors(c),gfx(gfx) {}

				size_t write(uint8_t ch) override {return gfx.write(ch);}

				void setColor(colorDefs c,bool selected=false,status s=enabledStatus) override {
					gfx.setTextColor(memWord(&colors[c][selected][s]));
				}

				void clearLine(idx_t ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus) override {
					gfx.fillRect(posX,posY+ln*resY,maxX*resX,resY,memWord(&colors[color][selected][stat]));
		    	//setCursor(0,ln);
		    }
		    void clear() override {
					gfx.fillScreen(memWord(&colors[bgColor][false][enabledStatus]));
		    	//setCursor(0,0);
		    }
		    void setCursor(idx_t x,idx_t y) override {gfx.setCursor(x*resX,y*resY);}

				void drawCursor(idx_t ln,bool selected,status stat) override {
					gfx.drawRect(posX,posY+ln*resY,maxX*resX,resY,memWord(&colors[cursorColor][selected][stat]));
				}

	  };

}; //namespace menuGFX
#endif
