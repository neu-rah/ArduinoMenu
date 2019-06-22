/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_XMLFMT
  #define RSITE_ARDUINO_MENU_XMLFMT

  #ifdef MENU_FMT_WRAPS
  #include "../menuDefs.h"
  #include "../items.h"
  // #include "esp8266Out.h"

    namespace Menu {

      // inline Print& operator<<(Print&o, Menu::prompt&p) {
      //   print_P(o,p.getText());
      //   return o;
      // }

      inline menuOut& operator<<(menuOut&o, idx_t i) {
        o.print(i);
        return o;
      }

      inline menuOut& operator<<(menuOut&o, const char* p) {
        print_P(o,p);
        return o;
      }

      // inline Print& operator<<(Print&o, const char* p) {
      //   print_P(o,p);
      //   return o;
      // }

      void outputOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx);

      //wraps a webserver output and writes xml to it
      template<class T>
      class xmlFmt:public T {
        public:
          using T::T;
          result fmt(bool start,prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) {
            trace(
              if (start) {
                *this<<"<!--xml fmt "<<part<<" idx:"<<idx<<(start?" start":" end");
                // if (target.has(_asPad)) *this<<" MENU_DEBUG=\"target_asPad\"";
                // if (nav.target->has(_asPad)) *this<<" MENU_DEBUG=\"nav_target_asPad\"";
                // if (&target==nav.target) *this<<" MENU_DEBUG=\"target_is_nav_target\"";
                *this<<"-->";
              });
            switch(part) {
              case menuOut::fmtPanel:
                if (start)
                  *this<<"\n<panel id=\""<<nav.target->hash()<<"\">";
                else T::operator<<("</panel>");
                break;
              case menuOut::fmtTitle:
                if (start) T::operator<<("<tit>");
                else T::operator<<("</tit>");
                break;
              case menuOut::fmtBody:
                if (start) {
                  *this<<"\n<menu r=\"";
                  nav.root->printPath(*this,(menuNode*)&target);
                  *this<<"\">";}
                else T::operator<<("</menu>");
                break;
              case menuOut::fmtUnit:
                *this<<(start?"<":"</")<<"u>";
                break;
              case menuOut::fmtOp:
                if (start) {
                  *this<<"\n<op";// id=\""<<target.hash()<<"\"";
                  if (target.has(_asPad)) *this<<" pad=\"y\"";
                  if (nav.sel==idx) *this<<" sel=\"y\"";
                  if (!target.enabled) *this<<" dis=\"y\"";
                  *this<<" i=\""<<idx<<"\"";
                  *this<<">";
                } else T::operator<<("</op>");
                break;
              case menuOut::fmtToggle:
                if (start) {
                  *this<<"<tog>";
                  outputOptions(*this,nav,*(menuNode*)&target,idx);
                  *this<<"</tog><fv><![CDATA[";
                } else *this<<"]]></fv>";
                break;
              case menuOut::fmtPrompt:
                if (start) {
                  *this<<"<p"
                    <<" t=\""<<target.typeName();
                    // <<"\" i=\""<<idx
                    // <<"\" ns=\""<<nav.sel
                    // <<"\" r=\"";
                    // nav.root->printPath(*this,nav.sel!=idx&&nav.target->has(_asPad)&&(&target!=nav.target)?-1:0);
                  // *this<<"/"<<idx;
                  *this<<"\"";
                  if (&target==nav.target) *this<<" act=\"y\"";
                  // if (nav.target->has(_asPad)) *this<<" DEBUG1=\"nav_target_asPad\"";
                  // if (&target==nav.target) *this<<" DEBUG2=\"target_is_nav_target\"";
                  *this<<"><![CDATA[";
                } else *this<<"]]></p>";
                break;
              case menuOut::fmtSelect:
                if (start) {
                  *this<<"<select>";
                  outputOptions(*this,nav,*(menuNode*)&target,idx);
                  *this<<"</select><fv><![CDATA[";
                } else *this<<"]]></fv>";
                break;
              case menuOut::fmtChoose:
                if (start) {
                  *this<<"<choose>";
                  outputOptions(*this,nav,*(menuNode*)&target,idx);
                  *this<<"</choose><fv><![CDATA[";
                } else *this<<"]]></fv>";
                break;
              case menuOut::fmtField:
                if (start) {
                  *this<<"<fld h=\"";
                  target.printHigh(*this);
                  *this<<"\" l=\"";
                  target.printLow(*this);
                  *this<<"\" s=\"";
                  target.printStep(*this);
                  *this<<"\" t=\"";
                  target.printTune(*this);
                  *this<<"\"><![CDATA[";
                } else *this<<"]]></fld>";
                break;
              case menuOut::fmtEditCursor:
                if (start) *this<<"<ec><![CDATA[";
                else *this<<"]]></ec>";
                break;
              case menuOut::fmtTextField:
                if (start) *this<<"<fv><![CDATA[";
                else *this<<"]]></fv>";
                break;
              case menuOut::fmtIdx:
                if (start) *this<<"<idx><![CDATA[";
                else *this<<"]]></idx>";
                break;
              case menuOut::fmtCursor:
              case menuOut::fmtCursorOpen:
              case menuOut::fmtCursorClose:
                if (start) *this<<"<cur><![CDATA[";
                else *this<<"]]></cur>";
                break;
              case menuOut::fmtOpBody:
                if (start) *this<<"<ob>";
                else *this<<"</ob>";
                break;
              case menuOut::fmtPreview:
                if (start) *this<<"<preview>";
                else *this<<"</preview>";
                break;
              default:break;
            }
            return proceed;
          }
          result fmtStart(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(true,target,part,nav,idx);}
          result fmtEnd(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(false,target,part,nav,idx);}
      };
    }//namespace
    #include "xmlFmt.hpp"
  #endif
#endif
