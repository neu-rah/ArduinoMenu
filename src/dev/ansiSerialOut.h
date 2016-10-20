#ifndef RSITE_ARDUINO_MENU_ANSISERIALOUT
  #define RSITE_ARDUINO_MENU_ANSISERIALOUT
  #include <AnsiStream.h>
  #include "serialOut.h"

  namespace Menu {

    class ansiSerialOut:public serialOut {
      public:
        const colorDef<uint8_t> (&colors)[nColors];
        inline ansiSerialOut(
          Print& o,
          const colorDef<uint8_t> (&c)[nColors],
          panelsList& p
        ) :serialOut(o,p,false,true),colors(c) {}
        inline uint8_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const {
          return memByte(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
        }
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
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          *this<<ANSI::xy(x+1+panels[panelNr].x,y+1+panels[panelNr].y);
        }
        void clear() override {
          *this<<ANSI::xy(maxX()+1,maxY())<<ANSI::eraseScreen()<<ANSI::xy(1,1);
        }
        void clear(idx_t panelNr) override {
          const panel p=panels[panelNr];
          fill(p.x,p.y,p.x+p.w,p.y+p.h);
          setCursor(0,0);
        }
        void clearLine(
          idx_t ln,
          colorDefs color=bgColor,
          bool selected=false,
          status stat=enabledStatus,
          bool edit=false,
          idx_t panelNr=0
        ) override {
          const panel p=panels[panelNr];
          *this<<ANSI::setBackgroundColor(getColor(color,selected,stat,edit));
          *this<<ANSI::fill(p.x,ln+p.y+1,p.x+p.w,p.y+ln+1);
        }
        /*void printMenu(navNode &nav,idx_t panelNr=0) override {
          idx_t ot=top;
          idx_t st=nav.root->showTitle?1:0;
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
            }
          }
          drawn=nav.target;
          //*this<<ANSI::reset();
        }*/
        void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {
          *this<<ANSI::setForegroundColor(getColor(c,selected,s,e));
        }
    };

  }//namespace Menu
#endif
