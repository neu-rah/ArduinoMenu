/********************
Use UTFT graphics display as menu output

www.r-site.net

Dec. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

UTFT library from:
  http://henningkarlsen.com/electronics/library.php?id=50

***/
#ifndef RSITE_ARDUINO_MENU_UTFT
	#define RSITE_ARDUINO_MENU_UTFT
	#include <UTFT.h>
	#include "menu.h"

  class menuUTFT:public menuOut {
    public:
  	uint16_t hiliteColor;
  	uint16_t bgColor;
  	uint16_t enabledColor;
  	uint16_t disabledColor;
  	uint16_t clearColor;
		uint16_t editCursorColor;
  	uint16_t curX,curY;//cursor x,y
    UTFT& gfx;
    menuUTFT(
    	UTFT& tft,
    	uint16_t hiliteColor=VGA_BLUE,
    	uint16_t bgColor=VGA_BLACK,
    	uint16_t enabledColor=VGA_WHITE,
    	uint16_t disabledColor=VGA_SILVER,
			uint16_t editCursorColoe=VGA_RED
    )
	  	:gfx(tft),
	  	bgColor(bgColor),
	  	enabledColor(enabledColor),
	  	disabledColor(disabledColor),
	  	hiliteColor(hiliteColor),
			editCursorColor(editCursorColor),
	  	clearColor(VGA_BLACK),
	  	curX(0),curY(0),
	  	menuOut()
  	 {}
  	 void init() {
  	   //setup geometry
       resX=gfx.getFontXsize();
       resY=gfx.getFontYsize();
       maxX=gfx.getDisplayXSize()/resX;
       maxY=gfx.getDisplayYSize()/resY;
  	 }

    virtual void clearLine(int ln) {
      gfx.setColor(bgColor);
    	gfx.fillRect(
    	  menuNode::activeNode->ox,
    	  menuNode::activeNode->oy+ln*resY,
    	  menuNode::activeNode->ox+resX*maxX-1,
    	  menuNode::activeNode->oy+ln*resY+resY-1
    	);
    	setCursor(0,ln);
    }
    virtual void clear() {
      gfx.setColor(clearColor);
    	gfx.fillRect(
    	  menuNode::activeNode->ox,
    	  menuNode::activeNode->oy,
    	  menuNode::activeNode->ox+resX*maxX-1,
    	  menuNode::activeNode->oy+resY*maxY-1
    	);
    	setCursor(0,0);
    }
	  virtual void setCursor(int x,int y) {
	    curX=menuNode::activeNode->ox+x*resX;
	    curY=menuNode::activeNode->oy+y*resY;
	  }
    virtual size_t write(uint8_t ch) {
      switch(ch) {
      case 0x0D://NL
        curX=0;
        break;
      case 0x0A://CR
        curY+=resY;
        break;
      default:
        gfx.printChar(ch,curX,curY);
        curX+=resX;
      }
      return 1;
    }
		virtual void drawEditCursor(int at,int posY) {
			//Serial<<"cursor at "<<at<<","<<posY<<endl;
			gfx.setColor(editCursorColor);
			gfx.drawRect(
				menuNode::activeNode->ox+at*resX,
    	  menuNode::activeNode->oy+posY*resY,
				menuNode::activeNode->ox+at*resX+1,
    	  menuNode::activeNode->oy+posY*resY+resY+1
			);
		}
    virtual void printPrompt(prompt &o,bool selected,int idx,int posX,int posY,int width) {
      gfx.setColor(selected?hiliteColor:bgColor);
    	gfx.fillRect(
    	  menuNode::activeNode->ox,
    	  menuNode::activeNode->oy+posY*resY,
    	  menuNode::activeNode->ox+maxX*resX-1,
    	  menuNode::activeNode->oy+posY*resY+resY-1
    	);
    	setCursor(0,posY);
      gfx.setBackColor(selected?hiliteColor:bgColor);
    	gfx.setColor(o.enabled?enabledColor:disabledColor);
    	o.printTo(*this);
    }
		virtual void printMenu(menu& m,bool drawExit) {
			if (drawn!=&m) clear();//clear screen box when changing menu
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
#endif
