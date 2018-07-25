/* -*- C++ -*- */

//this is a fossil, using xmlFmt instead

#ifndef RSITE_ARDUINO_MENU_HTMLFMT
  #define RSITE_ARDUINO_MENU_HTMLFMT
  #include "../menuDefs.h"

  namespace Menu {

    template<class T>
    class htmlFmt:public T {
      public:
        using T::T;
        result fmt(bool start,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) {
          assert(idx>=0&&idx<nav.sz());
          //prompt* n=&nav[idx];
          switch(part) {
            case menuOut::fmtPanel:
              if (start)
                *this
                  <<"<div id=\""<<nav.target->hash()
                  <<"\" class=\"aml_panel\">";
              else T::operator<<("</div>");
              break;
            case menuOut::fmtTitle:
              if (start) T::operator<<("<h2 class=\"aml_title\">");
              else T::operator<<("</h2>");
              break;
            case menuOut::fmtBody:
              if (start) T::operator<<("<ul class=\"aml_ops\">");
              else T::operator<<("</ul>");
              break;
            case menuOut::fmtOp:
              if (start) *this<<"<li class=\"aml_op\" data-idx=\""<<idx<<"\">";
              else T::operator<<("</li>");
              break;
            case menuOut::fmtToggle:
            case menuOut::fmtPrompt:
              if (idx>=0&&(nav[idx].type()==promptClass||nav[idx].type()==toggleClass))
                if (start) {
                  *this<<"<a class=\"aml_link\" href=\"/menu?at=";
                  nav.root->printPath(*this);
                  *this<<"/"<<idx<<"\">";
                } else *this<<"</a>";
              break;
            case menuOut::fmtSelect:
            case menuOut::fmtChoose:
              if(!start) break;
              *this<<"<select data-path=\"/menu?at=";
              nav.root->printPath(*this);
              *this<<"/"<<idx<<"\">";
              *this<<"\">";
              for(idx_t n=0;n<((menuNode*)&nav[idx])->sz();n++)
                *this
                  <<"<option"<<(n==nav[idx].selected()?" selected":"")<<">"
                  <<((menuNode*)&nav[idx])->operator[](n)<<"</option>";
              *this<<"</select>";
              return quit;
              break;
            case menuOut::fmtField:
              if (start) {
                *this<<"<input id=\""<<nav[idx].hash()<<"\" type=\"range\" data-idx=\""<<idx<<"\" min=\"";
                nav[idx].printLow(*this);
                *this<<"\" max=\"";
                nav[idx].printHigh(*this);
                *this<<"\" class=\"aml_field\" data-path=\"/menu?at=/"<<idx;
                nav.root->printPath(*this);
                *this<<"\" value=\"";
              } else *this<<"\">";
              break;
            /*case menuOut::fmtIdx:break;
            case menuOut::fmtCursor:break;
            case menuOut::fmtOpBody:break;
            case menuOut::fmtPreview:break;*/
            default:break;
          }
          return proceed;
        }
        result fmtStart(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(true,part,nav,idx);}
        result fmtEnd(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(false,part,nav,idx);}
    };
  }//namespace
#endif
