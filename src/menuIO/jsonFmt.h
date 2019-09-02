/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_JSONFMT
  #define RSITE_ARDUINO_MENU_JSONFMT

  #ifdef MENU_FMT_WRAPS
    #include "../menuDefs.h"
    #include "../items.h"
    // #include "esp8266Out.h"

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
                if (start) T::print("{");
                else T::print("}");
                break;
              case menuOut::fmtTitle:
                if (start) T::print("\"title\":{");
                else T::print("}\n");
                break;
              case menuOut::fmtBody:
                if (start) {
                  T::print(",\"path\":\"");
                  nav.root->printPath(*this,(menuNode*)&target);
                  T::print("\",\"sel\":\"");
                  T::print(nav.sel);
                  T::print("\"");
                  T::print(",\"items\":[\n");
                } else T::print("\n]\n");
                break;
              case menuOut::fmtUnit:
                if (start) T::print(",\"unit\":\"");
                else T::print("\"");
                break;
              case menuOut::fmtOp:
                if (start) {
                  if (idx>0) T::print(",\n");
                  T::print("{\"idx\":\"");
                  T::print(idx);
                  T::print("\"");
                  T::print(",\"enabled\":\"");
                  T::print(target.enabled);
                  T::print("\"");
                } else T::print("}");
                break;
              case menuOut::fmtToggle:
                if (start) {
                  T::print(",\"value\":\"");
                } else {
                  T::print("\"");
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                  // *this<<",";
                }
                break;
              case menuOut::fmtPrompt:
                if (start) {
                  if (idx>=0) T::print(",");
                  T::print("\"prompt\":\"");
                } else {
                  T::print("\"");
                  if (target.has(_asPad))
                    T::print(",\"pad\":[");
                };
                break;
              case menuOut::fmtValue:
                if (start) T::print(",\"value\":\"");
                else T::print("\"");
                break;
              case menuOut::fmtSelect:
                if (start) {
                  T::print(",\"select\":\"");
                } else {
                  T::print("\"");
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                }
                break;
              case menuOut::fmtChoose:
                if (start) {
                  T::print(",\"choose\":\"");
                } else {
                  T::print("\"");
                  jsonOptions(*this,nav,*(menuNode*)&target,idx);
                }
                break;
                break;
              case menuOut::fmtField:
                if (start) {
                  T::print(",\"field\":\"");
                } else {
                  T::print("\",\"range\":{\"low\":\"");
                  target.printLow(*this);
                  T::print("\",\"high\":\"");
                  target.printHigh(*this);
                  T::print("\",\"step\":\"");
                  target.printStep(*this);
                  T::print("\",\"tune\":\"");
                  target.printTune(*this);
                  T::print("\"}");
                }
                break;
              case menuOut::fmtEditCursor:
                if (start) T::print(",\"editCursor\":\"");
                else T::print("\"");
                break;
              case menuOut::fmtTextField:
                if (start) T::print(",\"text\":\"");
                else T::print("\"");
                break;
              case menuOut::fmtIdx:
                // if (start) *this<<"\"idx\":";
                // else *this<<",";
                break;
              case menuOut::fmtCursor:
                if (start) T::print(",\"cursor\":\"");
                else T::print("\"");
                break;
              case menuOut::fmtCursorOpen:
                if (start) T::print(",\"selStart\":\"");
                else T::print("\"");
                break;
              case menuOut::fmtCursorClose:
                if (start) T::print(",\"selEnd\":\"");
                else T::print("\"");
                break;
              case menuOut::fmtOpBody:
                if (start) {
                  // *this<<"\"body\":{";
                  // if (target.has(_asPad)) *this<<"\"pad\":[";
                } else {
                  if (target.has(_asPad)) T::print("]");
                  // *this<<"}";
                }
                break;
              case menuOut::fmtPreview:
                if (start) T::print(",\"preview\":\"");
                else T::print("\"\n");
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
    #include "jsonFmt.hpp"
  #endif
#endif
