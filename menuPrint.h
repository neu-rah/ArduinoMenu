/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no 
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu output to Print device (ex: Serial)
www.r-site.net
***/

#ifndef RSITE_ARDUINOP_MENU_PRINT
	#define RSITE_ARDUINOP_MENU_PRINT
	#include "menu.h"

  class menuPrint:public menuOut {
    public:
    Print& device;
    menuPrint(Print& device):device(device) {}
    virtual void clearLine(int ln) {}
    virtual void clear() {device.println("");device.println("");}
    virtual void setCursor(int x,int y) {device.println("");}
    virtual void print(char ch) {device.print(ch);}
    virtual void print(const char *text) {device.print(text);}
    virtual void println(const char *text="") {device.println(text);}
    virtual void print(int i) {device.print(i);};
    virtual void println(int i) {device.println(i);};
    virtual void print(double i) {device.print(i);};
    virtual void println(double i) {device.println(i);};
		virtual void print(prompt &o,bool selected,int idx,int posY,int width) {
      print(idx<10?" ":"");
      print(idx);
			//print(selected?(o.enabled?menu::enabledCursor:menu::disabledCursor):' ');
			o.printTo(*this,posY,selected);
			println();
		}
		virtual void printMenu(menu& m,bool drawExit) {
			if (drawn==&m&&m.sel==lastSel) return;
			clear();
			int i=0;
			for(;i<m.sz;i++)
			  print(*m.data[i],i==m.sel,i+1,i-top,m.width);
			if (drawExit) print(menu::exitOption,m.sel==m.sz,0,i-top,m.width);
			lastTop=top;
			lastSel=m.sel;
			drawn=&m;
		}
  };
  
#endif RSITE_ARDUINOP_MENU_PRINT

