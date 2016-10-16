#ifndef RSITE_ARDUINO_MENU_SERIALOUT
  #define RSITE_ARDUINO_MENU_SERIALOUT
  #include "../menu.h"

  namespace Menu {
    class serialOut:public menuOut {
      public:
        Print& device;
        idx_t lastLine=-1;
        inline serialOut(Print& o,idx_t mx=20,idx_t my=10):menuOut(mx,my,false,false),device(o) {}
        void clear() override {
          device<<endl;//device.println("");
        }
        void clearLine(idx_t ln,colorDefs color,bool selected=false,status stat=enabledStatus,bool edit=false) override {
          //device.println("");
        }
        size_t write(uint8_t ch) override {return device.write(ch);}
        void setCursor(idx_t x,idx_t y) override {
          if (lastLine!=y) {
            lastLine=y;
            device<<endl;
          }
        };
        //TODO: transpose enumeration as an option to all menus
        /*void printMenu(navNode &nav) override {
          idx_t ot=top;
          idx_t st=nav.root->showTitle?1:0;
          while(nav.sel+st>=(top+maxY)) top++;
          while(nav.sel<top||(top&&nav.sel+top<maxY-st)) top--;
          bool all=redraw||(top!=ot)||nav.target->dirty||drawn!=nav.target||nav.target->changed(nav,*this);
          if (all) {
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
            drawn=nav.target;
          }
        }*/

    };

  }//namespace Menu


#endif
