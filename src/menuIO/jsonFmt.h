/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_JSONFMT
  #define RSITE_ARDUINO_MENU_JSONFMT

  #ifdef MENU_FMT_WRAPS
  #include "../menuDefs.h"
  #include "../items.h"
  #include "esp8266Out.h"

    namespace Menu {

      void jsonOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx);

      //wraps a webserver output and writes xml to it
      template<class T>
      class jsonFmt:public T {
        public:
          using T::T;
          result fmt(bool start,prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) {
            switch(part) {
              case menuOut::fmtPanel:
                if (start) *this<<"{";
                else T::operator<<("}");
                break;
              case menuOut::fmtTitle:
                if (start) T::operator<<("\"title\":");
                else T::operator<<(",");
                break;
              case menuOut::fmtBody:
                if (start) {
                  *this<<"\"sel\":\""<<nav.sel<<"\",";
                  *this<<"\"items\": [";
                } else T::operator<<("],");      break;
              case menuOut::fmtUnit:
                *this<<(start?"\"unit\":":",");
                break;
              case menuOut::fmtOp:
                if (start) {
                  *this<<"{"
                    <<"\"op\":"
                    <<"\"idx\":\""<<idx<<"\","
                    <<"\"enabled\":"<<target.enabled<<"\",";
                } else T::operator<<("}");
                break;
              case menuOut::fmtToggle:
                if (start) {
                  *this<<"\"tog\":[";
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                  *this<<"],\"value\":";
                } else *this<<",";
                break;
              case menuOut::fmtPrompt:
                if (start) {
                  *this<<"\"tog\":";
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                  *this<<",\"value\":";
                } else {
                  *this<<",";
                  *this<<"\"name\":"<<target.typeName();
                  *this<<",";
                  *this<<"\"";
                };
                break;
              case menuOut::fmtSelect:
                if (start) {
                  *this<<"\"select\":";
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                } else *this<<",";
                break;
              case menuOut::fmtChoose:
                if (start) {
                  *this<<"\"choose\":";
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                } else *this<<",";
                break;
                break;
              case menuOut::fmtField:
                if (start) {
                  *this<<"\"field\":{\"high\":\"";
                  target.printHigh(*this);
                  *this<<"\",\"low\":\"";
                  target.printLow(*this);
                  *this<<"\",\"step\":\"";
                  target.printStep(*this);
                  *this<<"\",\"tune\":\"";
                  target.printTune(*this);
                  *this<<"\"}";
                } else *this<<",";
                break;
              case menuOut::fmtTextField:
                if (start) *this<<"\"value\":";
                else *this<<",";
                break;
              case menuOut::fmtIdx:
                if (start) *this<<"\"idx\":";
                else *this<<",";
                break;
              case menuOut::fmtCursor:
                if (start) *this<<"\"cursor\":";
                else *this<<",";
                break;
              case menuOut::fmtOpBody:
                if (start) *this<<"\"body\":";
                else *this<<",";
                break;
              case menuOut::fmtPreview:
                if (start) *this<<"\"preview\":";
                else *this<<",";
                break;
              default:break;
            }
            return proceed;
          }
          result fmtStart(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(true,target,part,nav,idx);}
          result fmtEnd(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(false,target,part,nav,idx);}
      };
    }//namespace
  #endif
#endif
