#ifndef RSITE_ARDUINOP_MENU_LCD
	#define RSITE_ARDUINOP_MENU_LCD
  #include <LiquidCrystal.h>
	#include "menu.h"

  class menuLCD:public menuOut {
    public:
    LiquidCrystal& lcd;
    //menuLCD(LiquidCrystal& lcd,int x=16,int y=1):lcd(lcd),menuOut(menuOut::cursor,x,y) {}
    menuLCD(LiquidCrystal& lcd,int x=16,int y=1):lcd(lcd),menuOut(x,y) {}
    virtual void clear() {lcd.clear();}
    virtual void setCursor(int x,int y) {lcd.setCursor(x*resX,y*resY);}
    virtual void print(char ch) {lcd.print(ch);}
    virtual void print(const char *text) {lcd.print(text);}
    virtual void println(const char *text) {lcd.print(text);};
    virtual void print(int i) {lcd.print(i);};
    virtual void println(int i) {lcd.println(i);};
		virtual void print(prompt &o,bool selected,int idx,int posY,int width) {
			lcd.setCursor(0,posY);
			print(selected?(o.enabled?menu::enabledCursor:menu::disabledCursor):' ');
			print(o.text);
		}
		virtual void printMenu(menu& m) {
			clear();
			if (m.sel-top>=maxY) top=m.sel-maxY+1;//selected option outside device (bottom)
			else if (m.sel<top) top=m.sel;//selected option outside device (top)
			int i=0;for(;i<m.sz;i++) {
				if ((i>=top)&&((i-top)<maxY)) {
				  if(i-top>=maxY) break;
				  print(*m.data[i],i==m.sel,i+1,i-top,m.width);
				}
			}
			if (i-top<maxY)
				print(menu::exitOption,m.sel==m.sz,0,i-top,m.width);
		}
  };
#endif RSITE_ARDUINOP_MENU_LCD

