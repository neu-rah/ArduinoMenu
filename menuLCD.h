/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no 
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extendable: Yes

Use standard arduino LCD (LiquidCrystal library) as menu output
***/

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
    virtual void println(const char *text="") {lcd.print(text);};
    virtual void print(int i) {lcd.print(i);};
    virtual void println(int i) {lcd.println(i);};
    virtual void print(double i) {lcd.print(i);};
    virtual void println(double i) {lcd.println(i);};
		virtual void print(prompt &o,bool selected,int idx,int posY,int width) {
			lcd.setCursor(0,posY);
			print(selected?(o.enabled?menu::enabledCursor:menu::disabledCursor):' ');
			//print(o.text);
			o.printTo(lcd);
			println();
		}
		/*virtual void print(menuField<int> &o,bool selected,int idx,int posY,int width) {
			println("Ok, this is it");
		}*/
		virtual void printMenu(menu& m,bool drawExit) {
			if (drawn==&m) return;
			clear();
			if (m.sel-top>=maxY) top=m.sel-maxY+1;//selected option outside device (bottom)
			else if (m.sel<top) top=m.sel;//selected option outside device (top)
			int i=0;for(;i<m.sz;i++) {
				if ((i>=top)&&((i-top)<maxY)) {
				  if(i-top>=maxY) break;
				  print(*m.data[i],i==m.sel,i+1,i-top,m.width);
				}
			}
			if (drawExit&&i-top<maxY)
				print(menu::exitOption,m.sel==m.sz,0,i-top,m.width);
			drawn=&m;
		}
  };
#endif RSITE_ARDUINOP_MENU_LCD

