#ifndef RSITE_ARDUINO_MENU_SERIALOUT
  #define RSITE_ARDUINO_MENU_SERIALOUT
  #include "../menu.h"

  namespace Menu {
    class serialOut:public menuOut {
      public:
        Print& device;
        idx_t lastLine=-1;
        inline serialOut(Print& o,panelsList &p,bool r=false,bool m=false)
          :menuOut(p,r,m),device(o) {
            drawNumIndex=true;
          }
        void clear() override {device<<endl;}
        void clear(idx_t panelNr) override {device<<endl;}
        void clearLine(
          idx_t ln,
          colorDefs color,
          bool selected=false,
          status stat=enabledStatus,
          bool edit=false,
          idx_t panelNr=0
        ) override {
          //device.println("");
        }
        size_t write(uint8_t ch) override {return device.write(ch);}
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          if (lastLine!=y) {
            lastLine=y;
            device<<endl;
          }
        };
    };

  }//namespace Menu


#endif
