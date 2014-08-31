#ifndef RSITE_ARDUINOP_MENU_PRINT
	#define RSITE_ARDUINOP_MENU_PRINT
  //#include <HardwareSerial.h>
	#include "menu.h"

  class menuPrint:public menuOut {
    public:
    Print& device;
    menuPrint(Print& device):device(device) {}
    virtual void clear() {device.println("");device.println("");}
    virtual void setCursor(int x,int y) {device.println("");}
    virtual void print(char ch) {device.print(ch);}
    virtual void print(const char *text) {device.print(text);}
    virtual void println(const char *text) {device.println(text);}
    virtual void print(int i) {device.print(i);};
    virtual void println(int i) {device.println(i);};
		virtual void print(prompt &o,bool selected,int idx,int posY,int width) {
			//setCursor(0,posY);
      print(idx<10?" ":"");
      print(idx);
			print(selected?(o.enabled?menu::enabledCursor:menu::disabledCursor):' ');
			println(o.text);
		}
  };
  
#endif RSITE_ARDUINOP_MENU_PRINT

