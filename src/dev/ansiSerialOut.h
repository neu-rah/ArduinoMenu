#ifndef RSITE_ARDUINO_MENU_ANSISERIALOUT
  #define RSITE_ARDUINO_MENU_ANSISERIALOUT
  #include <AnsiStream.h>
  #include "serialOut.h"

  namespace Menu {

    class ansiSerialOut:public serialOut {
      public:
        idx_t posX=0;
        idx_t posY=0;
        const uint8_t (&colors)[nColors][2][2];
        inline ansiSerialOut(Print& o,const uint8_t (&c)[nColors][2][2],idx_t x,idx_t y,idx_t px=0,idx_t py=0)
          :serialOut(o,x,y),posX(px),posY(py),colors(c) {}
        void setCursor(idx_t x,idx_t y) override {*this<<ANSI::xy(posX+x+1,posY+y+1);}
        void clear() override {*this<<ANSI::eraseScreen();}
        void clearLine(idx_t ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus) override {
          *this<<ANSI::setBackgroundColor(memByte(&colors[color][selected][stat]));
          *this<<ANSI::fill(posX+1,posY+ln+1,maxX-posX+1,posY+ln+1);
        }
        void printMenu(navNode &nav) override {
          idx_t ot=top;
          idx_t st=nav.root->showTitle?1:0;
          while(nav.sel+st>=(top+maxY)) top++;
          while(nav.sel<top) top--;
          bool all=(top!=ot)||nav.target->dirty||drawn!=nav.target;
          if (all) {
            clear();
            if (st) {
              setColor(titleColor,false);
              clearLine(0);
              setColor(titleColor,true);
              setCursor(0,0);
              *this<<"["<<*(prompt*)nav.target<<"]"<<endl;
              //*this<<ANSI::reset();
            }
          }
          for(idx_t i=0;i<maxY-st;i++) {
            int ist=i+st;
            if (i+top>=nav.sz()) break;
            prompt& p=nav[i+top];
            if (all||p.changed(nav,*this)) {
              bool selected=nav.sel==i+top;
              clearLine(ist,bgColor,selected,p.enabled);
              setCursor(0,ist);
              drawCursor(ist,selected,p.enabled);
              setColor(fgColor,selected,p.enabled);
              p.printTo(i+top,nav,*this);
              //*this<<ANSI::reset();
            }
          }
          drawn=nav.target;
          //*this<<ANSI::reset();
        }
        void setColor(colorDefs c,bool selected=false,status s=enabledStatus) override {
          *this<<ANSI::setForegroundColor(memByte(&colors[c][selected][s]));
        }
    };

  }//namespace Menu
#endif
