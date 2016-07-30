/**************
UTouch driver for arduino menu
turning utouch data into a menu input stream
checks menu option clicked or do menu scroll on drag

www.r-site.net

Dec. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extendable: Yes

UTouch library from:
  http://henningkarlsen.com/electronics/library.php?id=56
***/
#ifndef RSITE_ARDUINO_MENU_UTOUCH
#define RSITE_ARDUINO_MENU_UTOUCH

#include <UTouch.h>
#include "menu.h"

class menuUTouch:public Stream {
public:
  UTouch& touch;
  menuOut& out;
  int startX,startY;
  int scrlY;
  bool touching;
  bool dragging;
  unsigned long evTime;
  menuUTouch(UTouch& t,menuOut& o):touch(t),out(o),touching(false),dragging(false) {}
  int available(void) {return touch.dataAvailable()?1:touching;}
  int peek(void) {return -1;}
  int read() {
    menu& m=*out.drawn;
    if (touch.dataAvailable()) {
      evTime=millis();
      touch.read();
      startX=touch.getX()-menuNode::activeNode->ox;
      if (startX>out.maxX*out.resX) return -1;
      int y=touch.getY()-menuNode::activeNode->oy;
      if (y<0||y>out.maxY*out.resY) return -1;
      //within menu box
      if (touching) {//might be dragging
        int d=scrlY-y;
        int ad=abs(d);
        if (ad>(out.resY>>1)&&(ad<<1)>out.resY) {
          dragging=true;//we consider it a drag
          scrlY-=(d>0?1:-1)*(out.resY/2);
          return d>0?menu::upCode:menu::downCode;
        }
      }  else {//start new touching
        touching=true;
        dragging=false;
        startY=y;
        scrlY=y;
        evTime=millis();
      }
    } else {
      if (millis()-evTime<200) return -1;//debouncing
      touching=false;//touch ending
      if (dragging) return -1;
      if (menuNode::activeNode->isMenu()) {
        int at=startY/out.resY;
        return (at>=0&&at<(m.canExit?m.sz+1:m.sz))?(m.canExit&&at==m.sz?'0':at+out.top+'1'):-1;
      } else {//then its some sort of field
        menuNode* a=menuNode::activeNode;
        return strlen(a->text)*out.resX<startX?menu::enterCode:menu::escCode;
      }
    }
    return -1;
  }
  void flush() {}
  size_t write(uint8_t v) {return 0;}
};

#endif
