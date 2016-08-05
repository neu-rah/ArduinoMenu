/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

Use standard arduino LCD (LiquidCrystal library) as menu output

www.r-site.net
***/

#ifndef RSITE_ARDUINOP_MENU_LCD
	#define RSITE_ARDUINOP_MENU_LCD
  #include <LiquidCrystal.h>
	#include "menu.h"
	//#include "streamFlow.h"

  class menuLCD:public menuOut {
    public:
    LiquidCrystal& lcd;
    menuLCD(LiquidCrystal& lcd,int x=16,int y=1):lcd(lcd),menuOut(x,y) {}
    virtual void clearLine(int ln) {
    	setCursor(0,ln);
    	for(int n=0;n<maxX;n++) print(' ');
    	setCursor(0,ln);
    }
    virtual void clear() {lcd.clear();}
		virtual void showCursor() {lcd.cursor();}
		virtual void hideCursor() {lcd.noCursor();}
    virtual void setCursor(int x,int y) {lcd.setCursor(x*resX,y*resY);}
    virtual size_t write(uint8_t ch) {return lcd.write(ch);}
		virtual void printPrompt(prompt &o,bool selected,int idx,int posX, int posY,int width) {
			clearLine(posY);
			print(selected?(o.enabled?menu::enabledCursor:menu::disabledCursor):' ');
			o.printTo(*this);
		}
		virtual void printMenu(menu& m,bool drawExit) {
			if (drawn!=&m) clear();//clear screen when changing menu
			if (m.sel<top) top=m.sel;//selected option outside device (top)
			else if (m.sel-top>=maxY) top=m.sel-maxY+1;//selected option outside device (bottom)
			int i=0;for(;i<m.sz;i++) {
				if ((i>=top)&&((i-top)<maxY)) {
				  if (needRedraw(m,i)) {
				  	printPrompt(*(prompt*)pgmPtrNear(m.data[i]),i==m.sel,i+1,0,i-top,m.width);
				  }
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
