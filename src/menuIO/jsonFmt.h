/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_JSONFMT
  #define RSITE_ARDUINO_MENU_JSONFMT

  #ifdef MENU_FMT_WRAPS
    #include "../menuDefs.h"
    #include "../items.h"
    #include "esp8266Out.h"

    namespace Menu {

      #ifdef MENU_DEBUG
        extern int partIndent;
        const char* partName(menuOut::fmtParts part);
        void printIndent();
      #endif

      extern int cursorCtrl;

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
                if (start) T::operator<<("\"title\":{");
                else T::operator<<("}\n");
                break;
              case menuOut::fmtBody:
                if (start) {
                  *this<<",\"path\":\"";
                  nav.root->printPath(*this,(menuNode*)&target);
                  *this<<"\",\"sel\":\""<<nav.sel<<"\"";
                  *this<<",\"items\":[\n";
                } else T::operator<<("\n]\n");
                break;
              case menuOut::fmtUnit:
                *this<<(start?",\"unit\":\"":"\"");
                break;
              case menuOut::fmtOp:
                if (start) {
                  if (idx>0) *this<<",\n";
                  *this<<"{\"idx\":\""<<idx<<"\""
                    <<",\"enabled\":\""<<target.enabled<<"\"";
                } else *this<<("}");
                break;
              case menuOut::fmtToggle:
                if (start) {
                  *this<<",\"value\":\"";
                } else {
                  *this<<"\"";
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                  // *this<<",";
                }
                break;
              case menuOut::fmtPrompt:
                if (start) {
                  if (idx>=0) *this<<",";
                  *this<<"\"prompt\":\"";
                } else {
                  *this<<"\"";
                  if (target.has(_asPad))
                    *this<<",\"pad\":[";
                };
                break;
              case menuOut::fmtValue:
                if (start) *this<<",\"value\":\"";
                else *this<<"\"";
                break;
              case menuOut::fmtSelect:
                if (start) {
                  *this<<",\"select\":\"";
                } else {
                  *this<<"\"";
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                }
                break;
              case menuOut::fmtChoose:
                if (start) {
                  *this<<",\"choose\":\"";
                } else {
                  *this<<"\"";
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                }
                break;
                break;
              case menuOut::fmtField:
                if (start) {
                  *this<<",\"field\":\"";
                } else {
                  *this<<"\",\"range\":{\"low\":\"";
                  target.printLow(*this);
                  *this<<"\",\"high\":\"";
                  target.printHigh(*this);
                  *this<<"\",\"step\":\"";
                  target.printStep(*this);
                  *this<<"\",\"tune\":\"";
                  target.printTune(*this);
                  *this<<"\"}";
                }
                break;
              case menuOut::fmtEditCursor:
                if (start) *this<<",\"editCursor\":\"";
                else *this<<"\"";
                break;
              case menuOut::fmtTextField:
                if (start) *this<<",\"text\":\"";
                else *this<<"\"";
                break;
              case menuOut::fmtIdx:
                // if (start) *this<<"\"idx\":";
                // else *this<<",";
                break;
              case menuOut::fmtCursor:
                if (start) *this<<",\"cursor\":\"";
                else *this<<"\"";
                break;
              case menuOut::fmtCursorOpen:
                if (start) *this<<",\"selStart\":\"";
                else *this<<"\"";
                break;
              case menuOut::fmtCursorClose:
                if (start) *this<<",\"selEnd\":\"";
                else *this<<"\"";
                break;
              case menuOut::fmtOpBody:
                if (start) {
                  // *this<<"\"body\":{";
                  // if (target.has(_asPad)) *this<<"\"pad\":[";
                } else {
                  if (target.has(_asPad)) *this<<"]";
                  // *this<<"}";
                }
                break;
              case menuOut::fmtPreview:
                if (start) *this<<",\"preview\":\"";
                else *this<<"\"\n";
                break;
              default:break;
            }
            return proceed;
          }
          result fmtStart(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {
            trace(printIndent();MENU_DEBUG_OUT<<">"<<partName(part)<<" "<<idx<<" "<<target<<endl);
            #ifdef MENU_DEBUG
              partIndent++;
            #endif
            return fmt(true,target,part,nav,idx);
          }
          result fmtEnd(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {
            #ifdef MENU_DEBUG
              partIndent--;
            #endif
            trace(printIndent();MENU_DEBUG_OUT<<"<"<<partName(part)<<endl);
            return fmt(false,target,part,nav,idx);
          }
      };
    }//namespace
  #endif
#endif
