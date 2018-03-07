/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_XMLFMT
  #define RSITE_ARDUINO_MENU_XMLFMT

  #ifdef MENU_FMT_WRAPS
  #include "../menu.h"
  #include "../items.h"
  #include "esp8266Out.h"

    namespace Menu {

      inline Print& operator<<(Print&o, Menu::prompt&p) {
        print_P(o,p.getText());
        return o;
      }

      inline menuOut& operator<<(menuOut&o, idx_t i) {
        o.print(i);
        return o;
      }

      inline menuOut& operator<<(menuOut&o, const char* p) {
        print_P(o,p);
        return o;
      }

      inline Print& operator<<(Print&o, const char* p) {
        print_P(o,p);
        return o;
      }

      void outputOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx);

      //wraps a webserver output and writes xml to it
      template<class T>
      class xmlFmt:public T {
        public:
          using T::T;
          result fmt(bool start,prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) {
            trace(
              if (start) {
                *this<<"<!--xml fmt "<<part<<" idx:"<<idx<<(start?" start":" end\r\n");
                // if (target.has(_asPad)) *this<<" DEBUG=\"target_asPad\"";
                if (nav.target->has(_asPad)) *this<<" DEBUG=\"nav_target_asPad\"";
                if (&target==nav.target) *this<<" DEBUG=\"target_is_nav_target\"";
                *this<<"-->\r\n";
              });
            //prompt* n=&target;
            switch(part) {
              case menuOut::fmtPanel:
                if (start)
                  *this
                    <<"<panel id=\""<<target.hash()
                    <<"\" class=\"aml_panel\">";
                else T::operator<<("</panel>\r\n");
                break;
              case menuOut::fmtTitle:
                if (start) T::operator<<("<title class=\"aml_title\">");
                else T::operator<<("</title>\r\n");
                break;
              case menuOut::fmtBody:
                if (start) T::operator<<("<menu class=\"aml_ops\">");
                else T::operator<<("</menu>\r\n");
                break;
              case menuOut::fmtUnit:
                *this<<(start?"<":"</")<<"unit>";
                break;
              case menuOut::fmtOp:
                if (start) {
                  // assert(idx>=0&&idx<nav.sz());
                  *this
                    <<"<op"//data-type=\""<<target.typeName()
                    <<" class=\"aml_op op"<<target.type();
                    // <<"\" data-idx=\""<<idx
                    // <<"\" href=\"/menu?at=";
                  *this<<"\"";
                  if (nav.target->has(_asPad)) *this<<" DEBUG1=\"nav_target_asPad\"";
                  if (&target==nav.target) *this<<" DEBUG2=\"target_is_nav_target\"";
                  //   nav.root->printPath(*this,-1);
                  //   _trace(
                  //     Serial<<"DEBUG: xmlFmt skip asPad or parentDraw element! "<<idx<<endl;
                  //     *this<<"\" SKIP=\""<<idx;
                  //   );
                  // } else {
                    // nav.root->printPath(*this);
                    // *this<<"/"<<idx;
                  // }
                  *this<<">";
                } else T::operator<<("</op>\r\n");
                break;
              case menuOut::fmtToggle:
                if (start) {
                  *this<<"<toggle>";
                  outputOptions(*this,nav,*(menuNode*)&target,idx);
                  *this<<"</toggle>\r\n<field-value><![CDATA[";
                } else *this<<"]]></field-value>\r\n";
                break;
              case menuOut::fmtPrompt:
                if (start) {
                  *this<<"<prompt"
                    <<" data-type=\""<<target.typeName()
                    <<"\" data-idx=\""<<idx
                    <<"\" href=\"/menu?at=";
                  nav.root->printPath(*this,nav.target->has(_asPad)&&(&target!=nav.target)?-1:0);
                  *this<<"/"<<idx<<"\"";
                  if (nav.target->has(_asPad)) *this<<" DEBUG1=\"nav_target_asPad\"";
                  if (&target==nav.target) *this<<" DEBUG2=\"target_is_nav_target\"";
                  *this<<"><![CDATA[";
                } else *this<<"]]></prompt>\r\n";
                break;
              case menuOut::fmtSelect:
                if (start) {
                  // assert(idx>=0&&idx<nav.sz());
                  *this<<"<select>";
                  outputOptions(*this,nav,*(menuNode*)&target,idx);
                  *this<<"</select>\r\n<field-value><![CDATA[";
                } else *this<<"]]></field-value>\n\r";
                break;
              case menuOut::fmtChoose:
                if (start) {
                  *this<<"<choose>";
                  outputOptions(*this,nav,*(menuNode*)&target,idx);
                  *this<<"</choose>\r\n<field-value><![CDATA[";
                } else *this<<"]]></field-value>\n\r";
                break;
              case menuOut::fmtField:
                if (start) {
                  // assert(idx>=0&&idx<nav.sz());
                  *this<<"<field high=\"";
                  target.printHigh(*this);
                  *this<<"\" low=\"";
                  target.printLow(*this);
                  *this<<"\"><![CDATA[";
                } else *this<<"]]></field>\r\n";
                break;
              case menuOut::fmtTextField:
                if (start) *this<<"<field-value><![CDATA[";
                else *this<<"]]></field-value>\r\n";
                break;
              case menuOut::fmtIdx:
                if (start) *this<<"<idx><![CDATA[";
                else *this<<"]]></idx>\n\r";
                break;
              case menuOut::fmtCursor:
                if (start) *this<<"<cursor><![CDATA[";
                else *this<<"]]></cursor>";
                break;
              case menuOut::fmtOpBody:
                if (start) *this<<"<opBody>\n\r";
                else *this<<"</opBody>\r\n";
                break;
              case menuOut::fmtPreview:
                if (start) *this<<"<preview>";
                else *this<<"</preview>\r\n";
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
