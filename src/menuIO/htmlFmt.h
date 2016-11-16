/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_HTMLFMT
  #define RSITE_ARDUINO_MENU_HTMLFMT
  #include "../menu.h"

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
              if (start) T::operator<<("<div class=\"aml_panel\">");
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
              if (start) T::operator<<("<li class=\"aml_op\">");
              else T::operator<<("</li>");
              break;
            case menuOut::fmtToggle:
            case menuOut::fmtPrompt:
              if (idx>=0)
                if (start) {
                  *this<<"<a class=\"aml_link\" href=\"/menu?at=";
                  nav.root->printPath(*this);
                  *this<<"/"<<idx<<"\">";
                } else *this<<"</a>";
              break;
            case menuOut::fmtSelect:
              if (start) *this<<"choose«";
              else *this<<"»";
              break;
            case menuOut::fmtField:
              if (start) {
                *this<<"<input class=\"aml_field\" data-path=\"/menu?at=/"<<idx;
                nav.root->printPath(*this);
                *this<<"\" value=\"";
              } else *this<<"\">";
              break;
            case menuOut::fmtIdx:break;
            case menuOut::fmtCursor:break;
            case menuOut::fmtOpBody:
              switch(nav[idx].type()) {
                case chooseClass:
                  if(!start) break;
                  *this<<"<select>";
                  /*for(idx_t n=0;n<nav.sz();n++)
                    *this<<"<option"<<(n==idx?" selected":"")<<">"<<nav[idx][n].getText()<<"</option>";*/
                  *this<<"</select>";
                default: break;
              }
              if (start) *this<<"[opBody "<<(idx>=0?nav[idx].type():noClass);
              else *this<<"]";
              break;
            case menuOut::fmtPreview:break;
            default:break;
          }
          return proceed;
        }
        result fmtStart(menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {fmt(true,part,nav,idx);}
        result fmtEnd(menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {fmt(false,part,nav,idx);}
    };
  }//namespace
#endif
