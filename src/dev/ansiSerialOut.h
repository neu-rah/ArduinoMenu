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
        void setCursor(idx_t x,idx_t y) override {*this<<ANSI::xy(posX+x,posY+y);}
        void clear() override {*this<<ANSI::eraseScreen();}
        void clearLine(idx_t ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus) override {
          *this<<ANSI::setBackgroundColor(memByte(&colors[color][selected][stat]));
          *this<<ANSI::fill(posX,posY+ln,maxX-posX,posY+ln);
        }
        void printMenu(navNode &nav) override {
          if (nav.target->changed(nav,*this)) {
            setCursor(posX,posY);
            *this<<"["<<*(prompt*)nav.target<<"]::ANSI"<<endl;
            for(idx_t i=0;i<maxY;i++) {
              if (i+top>=nav.sz()) break;
              prompt& p=nav[i+top];
              bool selected=nav.sel==i+top;
              clearLine(i,bgColor,selected,p.enabled);
              setCursor(posX,posY+i);
              drawCursor(i,selected,p.enabled);
              setColor(fgColor,selected,p.enabled);
              p.printTo(i,nav,*this);
              //*this<<endl;
            }
          }
        }
        void setColor(colorDefs c,bool selected=false,status s=enabledStatus) override {
          *this<<ANSI::setForegroundColor(memByte(&colors[c][selected][s]));
        }
    };

  }//namespace Menu
#endif
