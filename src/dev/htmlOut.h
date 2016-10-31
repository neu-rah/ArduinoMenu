/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_HTMLOUT
  #define RSITE_ARDUINO_MENU_HTMLOUT
  #include "../menu.h"

  namespace Menu {
    class htmlOut:public menuOut {
      public:
        Print& device;
        inline serialOut(WiFiClient& o):menuOut(),device(o) {}
        size_t write(uint8_t ch) override {return device.write(ch);}
        void clear() override {}
        void clearLine(idx_t ln,colorDefs color,bool selected=false,status stat=enabledStatus,bool edit=false) override {}
        void setCursor(idx_t x,idx_t y) override {};
        void printMenu(navNode &nav) override {
          if (nav.target->changed(nav,*this)) {
            *this<<"["<<*(prompt*)nav.target<<"]"<<endl;
            for(idx_t i=0;i<maxY;i++) {
              if (i+top>=nav.sz()) break;
              *this<<"["<<i+1<<"]";
              prompt& p=nav[i+top];
              p.printTo(i,nav,*this);
            }
          }
        }

    };

  }//namespace Menu


#endif
