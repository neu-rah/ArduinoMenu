#ifndef RSITE_ARDUINO_MENU_SERIALOUT
  #define RSITE_ARDUINO_MENU_SERIALOUT
  #include "../menu.h"

  namespace Menu {
    class serialOut:public menuOut {
      public:
        Print& device;
        inline serialOut(Print& o):device(o) {}
        void clearLine(int ln) override {}
        void clear() override {device.println("");device.println("");}
        size_t write(uint8_t ch) override {return device.write(ch);}
        void printMenu(navNode &nav) override {
          if (nav.target->changed(nav,*this)) {
            *this<<"["<<*(prompt*)nav.target<<"]"<<endl;
            for(idx_t i=0;i<maxY;i++) {
              //if (i+top>=nav.sz()) break;
              *this<<"["<<i+1<<"]";
              prompt& p=nav[i];
              write(i==nav.sel?options.selectedCursor:' ');
              p.printTo(i,nav,*this);
              *this<<endl;
              p.dirty=false;
            }
            nav.target->prompt::dirty=false;
            for(int n=memStrLen((char*)memPtr(nav.target->shadow->text))+2;n;n--) *this<<"-";
            *this<<endl;
          }
        }
    };
  }//namespace Menu


#endif
