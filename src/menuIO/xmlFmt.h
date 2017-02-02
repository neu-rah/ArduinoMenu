/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_XMLFMT
  #define RSITE_ARDUINO_MENU_XMLFMT
  #include "../menu.h"

  namespace Menu {

    menuOut& operator<<(menuOut&o,classes c) {
      switch(c) {
        case noClass:return o<<"noClass";break;
        case promptClass:return o<<"prompt";break;
        case fieldClass:return o<<"field";break;
        case toggleClass:return o<<"toggle";break;
        case selectClass:return o<<"select";break;
        case chooseClass:return o<<"choose";break;
        case valueClass:return o<<"value";break;
        case menuClass:return o<<"menu";break;
        default: return o;
      }
    }

    void outputOptions(menuOut& o,navNode &nav,menuNode& node,idx_t idx) {
      o<<"<node data-path=\"/menu?at=";
      nav.root->printPath(o);
      o<<"/"<<idx<<"\">";
      for(idx_t n=0;n<node.sz();n++)
        o<<"<value"<<(n==node.selected()?" selected=\"selected\"":"")<<">"<<node[n]<<"</value>";
      o<<"</node>";
    }

    template<class T>
    class xmlFmt:public T {
      public:
        using T::T;
        result fmt(bool start,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) {
          assert(idx>=0&&idx<nav.sz());
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
                *this<<"<op class=\"aml_op op"<<nav[idx].type()<<"\" data-idx=\""<<idx<<"\" href=\"/menu?at=";
                nav.root->printPath(*this);
                *this<<"/"<<idx<<"\">";
              } else T::operator<<("</op>\r\n");
              break;
            case menuOut::fmtToggle:
              if (start) *this<<"<toggle><![CDATA[";
              else *this<<"]]></toggle>\r\n";
              break;
            case menuOut::fmtPrompt:
              if (start) *this<<"<prompt><![CDATA[";
              else *this<<"]]></prompt>\r\n";
              break;
            case menuOut::fmtSelect:
              if (start) {
                *this<<"<select>";
                outputOptions(*this,nav,*(menuNode*)&nav[idx],idx);
                *this<<"<![CDATA[";
              } else *this<<"]]></select>\r\n";
              break;
            case menuOut::fmtChoose:
              if (start) *this<<"<choose><![CDATA[";
              else *this<<"]]></choose>\r\n";
              break;
            case menuOut::fmtField:
              if (start) {
                *this<<"<field>";
                outputOptions(*this,nav,*(menuNode*)&nav[idx],idx);
                *this<<"<![CDATA[";
              }
              else *this<<"]]></field>\r\n";
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
