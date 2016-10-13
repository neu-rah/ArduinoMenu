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

    class menuU8G:public gfxOut {
      public:
	      U8GLIB& gfx;
				int8_t fontMargin=2;
				const colorDef<uint8_t> (&colors)[nColors];
	      menuU8G(U8GLIB& gfx,const colorDef<uint8_t> (&c)[nColors],idx_t resX=6,idx_t resY=9)
					:gfxOut(gfx.getWidth()/resX,gfx.getHeight()/resY,resX,resY,true),colors(c),gfx(gfx) {
	        	gfx.setFontPosBottom(); // U8Glib font positioning
	      }

				size_t write(uint8_t ch) override {
	          gfx.write(ch); // print the ASCII correspoding char instead of print which display the decimal value of the char.
	          return 1;
	      }

				inline uint8_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
          return memByte(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
        }

				void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false) override {
					gfx.setColorIndex(getColor(c,selected,s,edit));
				}

				void clearLine(idx_t ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
						setColor(color,selected,stat,edit);
						gfx.drawBox(posX,posY+ln*resY,maxX*resX,resY);
	          //setCursor(0,ln);
	      }
	      void clear() override {
	          // No need to clear, the U8Glib display loop clear screen on each refresh
	      }
	      void setCursor(idx_t x,idx_t y) override {
	          gfx.setPrintPos(posX+x*resX,posY+(y+1)*resY-fontMargin);
	      }

				void drawCursor(idx_t ln,bool selected,status stat,bool edit=false) override {
					gfxOut::drawCursor(ln,selected,stat);
					setColor(cursorColor,selected,stat);
					gfx.drawFrame(posX,posY+ln*resY,maxX*resX,resY);
				}
  	};
}
#endif RSITE_ARDUINOP_MENU_LCD
