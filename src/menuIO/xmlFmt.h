/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_XMLFMT
  #define RSITE_ARDUINO_MENU_XMLFMT

  #ifdef MENU_FMT_WRAPS
  #include "../menu.h"
  #include "../items.h"

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

      template<class T>
      class xmlFmt:public T {
        public:
          using T::T;
          result fmt(bool start,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) {
            trace(Serial<<"xml fmt "<<part<<" idx:"<<idx<<(start?" start":" end")<<endl);
            //prompt* n=&nav[idx];
            switch(part) {
              case menuOut::fmtPanel:
                if (start)
                  *this
                    <<"<panel id=\""<<nav.target->hash()
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
              case menuOut::fmtOp:
                if (start) {
                  assert(idx>=0&&idx<nav.sz());
                  *this<<"<op data-type=\""<<nav[idx].typeName()<<"\" class=\"aml_op op"<<nav[idx].type()<<"\" data-idx=\""<<idx<<"\" href=\"/menu?at=";
                  nav.root->printPath(*this);
                  *this<<"/"<<idx<<"\">";
                } else T::operator<<("</op>\r\n");
                break;
              case menuOut::fmtToggle:
                if (start) {
                  *this<<"<toggle>";
                  outputOptions(*this,nav,*(menuNode*)&nav[idx],idx);
                  *this<<"</toggle>\r\n<field-value><![CDATA[";
                } else *this<<"]]></field-value>\r\n";
                break;
              case menuOut::fmtPrompt:
                if (start) *this<<"<prompt><![CDATA[";
                else *this<<"]]></prompt>\r\n";
                break;
              case menuOut::fmtSelect:
                if (start) {
                  assert(idx>=0&&idx<nav.sz());
                  *this<<"<select>";
                  outputOptions(*this,nav,*(menuNode*)&nav[idx],idx);
                  *this<<"</select>\r\n<field-value><![CDATA[";
                } else *this<<"]]></field-value>";
                break;
              case menuOut::fmtChoose:
                if (start) {
                  *this<<"<choose>";
                  outputOptions(*this,nav,*(menuNode*)&nav[idx],idx);
                  *this<<"</choose>\r\n<field-value><![CDATA[";
                } else *this<<"]]></field-value>";
                break;
              case menuOut::fmtField:
                if (start) {
                  assert(idx>=0&&idx<nav.sz());
                  *this<<"<field high=\"";
                  nav[idx].printHigh(*this);
                  *this<<"\" low=\"";
                  nav[idx].printLow(*this);
                  *this<<"\"><![CDATA[";
                } else *this<<"]]></field>\r\n";
                break;
              case menuOut::fmtTextField:
                if (start) *this<<"<field-value><![CDATA[";
                else *this<<"]]></field-value>\r\n";
                break;
              case menuOut::fmtIdx:
                if (start) *this<<"<idx><![CDATA[";
                else *this<<"]]></idx>";
                break;
              case menuOut::fmtCursor:
                if (start) *this<<"<cursor><![CDATA[";
                else *this<<"]]></cursor>";
                break;
              case menuOut::fmtOpBody:
                if (start) *this<<"<opBody>";
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
          result fmtStart(menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(true,part,nav,idx);}
          result fmtEnd(menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(false,part,nav,idx);}
      };
    }//namespace
  #endif
#endif
