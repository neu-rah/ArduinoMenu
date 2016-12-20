/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
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
//#include <U8g_teensy.h>
	#include "menu.h"

  class menuU8G:public menuOut {
    public:
  	unsigned char hiliteColor;
  	unsigned char bgColor;
  	unsigned char enabledColor;
  	unsigned char disabledColor;
		unsigned char enabledColorHi;
		unsigned char disabledColorHi;
		unsigned char valueColor;
		unsigned char valueColorHi;

    U8GLIB& gfx;
    menuU8G(
    	U8GLIB& gfx,
    	unsigned char hiliteColor=3,
    	unsigned char bgColor=0,
    	unsigned char enabledColor=2,
    	unsigned char disabledColor=1,
			unsigned char valueColor=2,
			unsigned char valueColorHi=2,
    	uint8_t resX = 7,//6 font width
    	uint8_t resY = 8 //9 font height + 2 pixels of spacing
    )
	  	:gfx(gfx),
	  	bgColor(bgColor),
	  	enabledColor(enabledColor),
	  	disabledColor(disabledColor),
	  	hiliteColor(hiliteColor),
			enabledColorHi(bgColor),
	  	disabledColorHi(bgColor),
			valueColor(valueColor),
			valueColorHi(valueColorHi),
	  	menuOut(gfx.getWidth()/resX,gfx.getHeight()/resY,resX,resY)
        {
            // Small typefaces used to draw the menu, do not forget to report resX and resY
            //gfx.setFont(u8g_font_6x10r);
            //gfx.setFont(u8g_font_baby);
            //gfx.setFont(u8g_font_profont10r);
            //gfx.setFont(u8g_font_trixel_square);
            //gfx.setFont(u8g_font_lucasfont_alternate);
            gfx.setFont(u8g_font_04b_03); // Good result
            gfx.setFontPosBottom(); // U8Glib font positioning
        }

    virtual void clearLine(int ln) {
        // No need to clear, the U8Glib display loop clear screen on each refresh
        //setCursor(0,ln);
    }
    virtual void clear() {
        // No need to clear, the U8Glib display loop clear screen on each refresh
    }
    virtual void setCursor(int x,int y) {
        signed char xPxTextOffset = 2; //  x offset in pixels on text
        signed char yPxTextOffset = -1; // y offset in pixels on text
        gfx.setPrintPos(x*resX+xPxTextOffset,y*resY+yPxTextOffset);
    }
    virtual size_t write(uint8_t ch) {
        gfx.write(ch); // print the ASCII correspoding char instead of print which display the decimal value of the char.
        return 1;
    }
    virtual void printPrompt(prompt &o,bool selected,int idx,int posX,int posY,int width) {
        gfx.setColorIndex(selected?hiliteColor:bgColor);
        gfx.drawBox(posX*resX,posY*resY,maxX*resX,resY);
        gfx.setColorIndex(selected?(o.enabled?enabledColorHi:disabledColorHi):(o.enabled?enabledColor:disabledColor));
        setCursor(posX,posY+1); //+1 compensate the height of the font and the way how U8Glib works
        //o.printTo(*this);
				o.printName(*this);
				gfx.setColorIndex(selected?valueColorHi:valueColor);
				o.printValue(*this);
				o.printUnit(*this);
    }
		virtual void printMenu(menu& m,bool drawExit) {
			//Serial<<"printing menu "<<m<<" top:"<<top<<endl;
			//Serial<<*menu::activeNode<<" <-> "<<*drawn<<endl;
			if (m.sel < top) {
				top = m.sel;//+ resY; //selected option outside device (top)
			} else if (m.sel-top >= maxY) {
        top = m.sel - maxY + 1;//resY-1; //selected option outside device (bottom)
			}
      int i = top;
      for (; i < m.sz; i++) {
			  if (i-top >= maxY) break;
			  if (needRedraw(m,i))
					printPrompt(*(prompt*)pgmPtrNear(m.data[i]),i == m.sel,i+1,m.ox,(i-top)+m.oy,m.width);
      	//printPrompt(*m.data[i],i == m.sel,i+1,m.ox,(i-top)+m.oy,m.width);
			}
			if (drawExit && i-top<maxY && needRedraw(m,i))
				printPrompt(menu::exitOption,m.sel==m.sz,0,m.ox,(i-top)+m.oy,m.width);
			lastTop = top;
			lastSel = m.sel;
			//Serial<<"menuU8G::printMenu top:"<<top<<endl;
			//drawn=&m;  // if commented, equals to have gfx.redraw() in loop - not ok for U8Glib
		}
  };
#endif RSITE_ARDUINOP_MENU_LCD
