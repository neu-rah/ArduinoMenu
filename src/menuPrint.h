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
    virtual size_t write(uint8_t ch) {return device.write(ch);}
		virtual void printPrompt(prompt &o,bool selected,int idx,int posX,int posY,int width) {
      print(idx<10?" ":"");
      print((unsigned long)idx);
			print(selected?(o.enabled?menu::enabledCursor:menu::disabledCursor):' ');
			o.printTo(*this);
			println();
		}
		virtual void printMenu(menu& m,bool drawExit) {
			bool any=false;
			for(int i=0;i<m.sz;i++) any|=m[i].needRedraw(*this,i==m.sel);
			if ((!any)&&drawn==&m&&m.sel==lastSel) return;
			clear();
			int i=0;
			for(;i<m.sz;i++)
        printPrompt(*(prompt*)pgmPtrNear(m.data[i]),i==m.sel,i+1,0,i-top,m.width);
      if (drawExit) printPrompt(menu::exitOption,m.sel==m.sz,0,0,i-top,m.width);
			lastTop=top;
			lastSel=m.sel;
			drawn=&m;
		}
  };

#endif RSITE_ARDUINOP_MENU_PRINT
