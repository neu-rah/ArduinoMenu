#ifndef RSITE_ARDUINO_MENU_ANSISERIALOUT
  #define RSITE_ARDUINO_MENU_ANSISERIALOUT
  #include <AnsiStream.h>
  #include "serialOut.h"

  namespace Menu {

    class ansiSerialOut:public serialOut {
      public:
        idx_t posX=0;
        idx_t posY=0;
        const colorDef<uint8_t> (&colors)[nColors];
        inline uint8_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
          return memByte(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
        }
        inline ansiSerialOut(Print& o,const colorDef<uint8_t> (&c)[nColors],idx_t x,idx_t y,idx_t px=0,idx_t py=0)
          :serialOut(o,x,y),posX(px),posY(py),colors(c) {}
        menuOut& fill(
          int x1, int y1, int x2, int y2,char ch=' ',
          colorDefs color=bgColor,
          bool selected=false,
          status stat=enabledStatus,
          bool edit=false) override {
            *this<<ANSI::setBackgroundColor(getColor(color,selected,stat,edit));
            *this<<ANSI::fill(x1+1,y1+1,x2+1,y2+1,ch);
            return *this;
        }
        void setCursor(idx_t x,idx_t y) override {*this<<ANSI::xy(posX+x+1,posY+y+1);}
        void clear() override {*this<<ANSI::eraseScreen()<<ANSI::xy(1,1);}
        void clearLine(idx_t ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override {
          *this<<ANSI::setBackgroundColor(getColor(color,selected,stat,edit));
          *this<<ANSI::fill(posX+1,posY+ln+1,maxX-posX+1,posY+ln+1);
        }
        void printMenu(navNode &nav) override {
          idx_t ot=top;
          idx_t st=nav.root->showTitle?1:0;
          /*Serial<<ANSI::xy(0,20)<<ANSI::setBackgroundColor(BLUE)<<ANSI::setForegroundColor(YELLOW)
            <<"sel:"<<nav.sel<<" top:"<<top<<" "<<(nav.sel+1==nav.sz())<<" "<<(nav.sel-top)<<" "<<(maxY-st);*/
          while(nav.sel+st>=(top+maxY)) top++;
          while(nav.sel<top||(top&&nav.sel+top<maxY-st)) top--;
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
        void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {
          *this<<ANSI::setForegroundColor(getColor(c,selected,s,e));
        }
    };

  }//namespace Menu
#endif
