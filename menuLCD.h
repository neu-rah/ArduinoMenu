#ifndef RSITE_ARDUINOP_MENU_LCD
	#define RSITE_ARDUINOP_MENU_LCD
  #include <LiquidCrystal.h>
	#include "menu.h"

  class menuLCD:public menuOut {
    public:
    LiquidCrystal& lcd;
    menuLCD(LiquidCrystal& lcd,int x=16,int y=1):lcd(lcd),menuOut(menuOut::cursor,x,y) {}
    virtual void clear() {lcd.clear();}
    virtual void setCursor(int x,int y) {lcd.setCursor(x*resX,y*resY);}
    virtual void print(char ch) {lcd.print(ch);}
    virtual void print(const char *text) {lcd.print(text);}
    virtual void println(const char *text) {lcd.print(text);};
    virtual void print(int i) {lcd.print(i);};
    virtual void println(int i) {lcd.println(i);};
    //static operator menuOut(LiquidCrystal &lcd) {return menuLCD(lcd);}
  };
#endif RSITE_ARDUINOP_MENU_LCD

