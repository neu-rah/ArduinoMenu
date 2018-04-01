/* -*- C++ -*- */
/********************
Oct. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

Use ANSI codes over serial output
www.r-site.net

***/
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
          idx_t* t,
          panelsList& p
        ) :serialOut(o,t,p,menuOut::minimalRedraw),colors(c) {}
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
          *this<<ANSI::xy(panels.maxX()+1,panels.maxY()+1)<<ANSI::eraseScreen()<<ANSI::xy(1,1);
          panels.reset();
        }
        void clear(idx_t panelNr) override {
          const panel p=panels[panelNr];
          fill(p.x,p.y,p.x+p.w-1,p.y+p.h-1);
          setCursor(0,0,panelNr);
          panels.nodes[panelNr]=NULL;
        }
        void clearLine(
          idx_t ln,
          idx_t panelNr=0,
          colorDefs color=bgColor,
          bool selected=false,
          status stat=enabledStatus,
          bool edit=false
        ) override {
          const panel p=panels[panelNr];
          *this<<ANSI::setBackgroundColor(getColor(color,selected,stat,edit));
          *this<<ANSI::fill(p.x+1,ln+p.y+1,p.x+p.w,p.y+ln+1);
        }
        void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {
          *this<<ANSI::setForegroundColor(getColor(c,selected,s,e));
        }
        virtual idx_t startCursor(bool charEdit) {
          if (charEdit) *this<<ANSI::inverseOn();
          else *this<<ANSI::boldOn();
          return 0;
        }
        virtual idx_t endCursor(bool charEdit) {
          if (charEdit) *this<<ANSI::inverseOff();
          else *this<<ANSI::boldOff();
          return 0;
        }
        virtual idx_t editCursor(idx_t x,idx_t y,bool editing,bool charEdit) {return 0;}
    };

  }//namespace Menu
#endif
