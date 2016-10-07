#ifndef RSITE_ARDUINO_MENU_SERIALOUT
  #define RSITE_ARDUINO_MENU_SERIALOUT
  #include "../menu.h"
  #include "../Dump.h"

  namespace Menu {
    class serialOut:public menuOut {
      public:
        Print& device;
        inline serialOut(Print& o,idx_t mx,idx_t my):menuOut(mx,my),device(o) {}
        void clear() override {
          device.println("");device.println("");
        }
        void clearLine(idx_t ln,colorDefs color,bool selected=false,status stat=enabledStatus) override {
          device.println("");
        }
        size_t write(uint8_t ch) override {return device.write(ch);}
        void setCursor(idx_t x,idx_t y) override {};
        void printMenu(navNode &nav) override {
          if (nav.target->changed(nav,*this)) {
            *this<<"["<<*(prompt*)nav.target<<"]"<<endl;
            for(idx_t i=0;i<maxY;i++) {
              if (i+top>=nav.sz()) break;
              *this<<"["<<i+1<<"]";
              prompt& p=nav[i+top];
              drawCursor(i,i==nav.sel,p.enabled);
              //write(i==nav.sel?options.selectedCursor:' ');
              p.printTo(i,nav,*this);
              *this<<endl;
            }
            for(int n=memStrLen((char*)memPtr(nav.target->shadow->text))+2;n;n--) *this<<"-";
            *this<<endl;
          }
        }
    };

  }//namespace Menu


#endif
