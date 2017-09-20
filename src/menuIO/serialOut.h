/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_SERIALOUT
  #define RSITE_ARDUINO_MENU_SERIALOUT
  #include "../menu.h"

  namespace Menu {

    extern panelsList default_serial_panel_list;

    class serialOut:public menuOut {
      public:
        Print& device;
        idx_t lastLine=-1;
        inline serialOut(Print& o,idx_t* t,panelsList &p=default_serial_panel_list,menuOut::styles st=menuOut::none)
          :menuOut(t,p,(menuOut::styles)(st|menuOut::drawNumIndex)),device(o) {}
        void clear() override {
          println();
          panels.reset();
        }
        void clear(idx_t panelNr) override {
          println();
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
          lastLine=-1;
          device.println("");
        }
        size_t write(uint8_t ch) override {return device.write(ch);}
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
          if (lastLine>=0&&lastLine!=y) println();
          lastLine=y;
        };
    };

  }//namespace Menu


#endif
